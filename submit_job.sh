#!/bin/bash
# Executive Summary:
# 
# This script is designed to facilitate the submission of jobs to a remote server for the generation of behavior trees. 
# It leverages SSH and SCP commands to manage the connection, copy necessary files, and submit the job. 
# 
# The script also includes functionality to:
# 
# - Verify the SSH connection
# - Wait for job completion
# - Copy the results back to the local machine
# 
# It is intended to be used in conjunction with the Behavior Tree Viewer application for generating and managing behavior trees.

# Load environment variables from .env file
# This section loads environment variables from a .env file if it exists.
if [ -f .env ]; then
    export $(cat .env | grep -v '#' | xargs)
else
    echo "Error: .env file not found"
    exit 1
fi

# Required environment variables check
# This section checks if all required environment variables are set in the .env file.
if [ -z "$REMOTE_HOST" ] || [ -z "$REMOTE_USER" ] || [ -z "$SSH_KEY_PATH" ] || [ -z "$REMOTE_DIR" ]; then
    echo "Error: Required environment variables not set in .env file"
    echo "Please ensure REMOTE_HOST, REMOTE_USER, SSH_KEY_PATH, and REMOTE_DIR are set"
    exit 1
fi

# Check for scenarios file
# This section checks if a scenarios file is provided as a command line argument.
if [ -z "$1" ]; then
    echo "Error: No scenarios file provided"
    echo "Usage: $0 scenarios.json [output_dir]"
    exit 1
fi

# Set scenarios file and output directory
# This section sets the scenarios file and output directory based on command line arguments.
SCENARIOS_FILE="$1"
OUTPUT_DIR="${2:-behavior_trees}"

# Check if scenarios file exists
# This section checks if the provided scenarios file exists.
if [ ! -f "$SCENARIOS_FILE" ]; then
    echo "Error: Scenarios file '$SCENARIOS_FILE' not found"
    exit 1
fi

# SSH commands
# This section sets up SSH and SCP commands for remote operations.
SSH_CMD="ssh -i $SSH_KEY_PATH ${REMOTE_USER}@${REMOTE_HOST}"
SCP_CMD="scp -i $SSH_KEY_PATH"

# Function to verify SSH connection
# This function verifies the SSH connection to the remote server.
verify_ssh_connection() {
    echo "Verifying SSH connection to ${REMOTE_HOST}..."
    if $SSH_CMD "echo 'SSH connection successful' && exit" >/dev/null 2>&1; then
        echo "✅ SSH connection verified successfully"
        return 0
    else
        echo "❌ Failed to establish SSH connection"
        return 1
    fi
}

# Function to submit job
# This function submits a job to the remote server for behavior tree generation.
submit_job() {
    if ! verify_ssh_connection; then
        exit 1
    fi
    
    echo "Creating necessary directories on remote server..."
    $SSH_CMD "mkdir -p ${REMOTE_DIR}"
    
    echo "Copying files to remote server..."
    # Copy the Slurm script
    $SCP_CMD run_model.slurm ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    # Copy the scenarios file
    $SCP_CMD "$SCENARIOS_FILE" ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    # Copy Python scripts if they exist locally
    for script in demo_ssh.py retrieve_context.py; do
        if [ -f "$script" ]; then
            $SCP_CMD "$script" ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/codellama-bt-adapter/
        fi
    done
    # Copy .env file if needed
    if [ -f ".env" ]; then
        $SCP_CMD .env ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/codellama-bt-adapter/
    fi
    
    echo "Submitting SLURM job..."
    SLURM_RESPONSE=$($SSH_CMD "cd ${REMOTE_DIR} && sbatch run_model.slurm")
    JOB_ID=$(echo $SLURM_RESPONSE | grep -oP "Submitted batch job \K\d+")
    
    if [ ! -z "$JOB_ID" ]; then
        echo "Job submitted successfully: $JOB_ID"
        
        # Wait for job completion
        echo "Waiting for job completion..."
        while true; do
            sleep 30
            JOB_STATE=$($SSH_CMD "squeue -j ${JOB_ID} -h -o %t" 2>/dev/null)
            if [ -z "$JOB_STATE" ]; then
                break
            fi
            echo "Job state: $JOB_STATE"
        done
        
        # Create local output directory
        mkdir -p "$OUTPUT_DIR"/{xml,metadata}
        
        echo "Copying results back to local machine..."
        # The correct path should include codellama-bt-adapter and the specific job ID
        REMOTE_RESULTS_DIR="${REMOTE_DIR}/codellama-bt-adapter/runs/${JOB_ID}"
        
        # Copy XML files
        echo "Copying XML files..."
        $SCP_CMD -r "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_RESULTS_DIR}/outputs/xml/*" "$OUTPUT_DIR/xml/" || {
            echo "⚠️  Warning: No XML files found"
        }
        
        # Copy metadata files
        echo "Copying metadata files..."
        $SCP_CMD -r "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_RESULTS_DIR}/outputs/metadata/*" "$OUTPUT_DIR/metadata/" || {
            echo "⚠️  Warning: No metadata files found"
        }
        
        # Copy logs
        echo "Copying logs..."
        $SCP_CMD -r "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_RESULTS_DIR}/logs" "$OUTPUT_DIR/" || {
            echo "⚠️  Warning: No logs found"
        }
        
        echo "✅ Job completed. Results copied to $OUTPUT_DIR"
        echo "Generated files:"
        ls -R "$OUTPUT_DIR"
    else
        echo "❌ Failed to submit job"
        exit 1
    fi
}

# Parse command line argument
# This section parses the command line argument to determine the action to take.
case "$1" in
    -v|--verify)
        verify_ssh_connection
        exit
        ;;
    *.json)
        submit_job
        ;;
    *)
        echo "Usage: $0 scenarios.json [output_dir]"
        echo "  or: $0 [-v|--verify]"
        exit 1
        ;;
esac