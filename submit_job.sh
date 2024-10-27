#!/bin/bash

# Load environment variables from .env file
if [ -f .env ]; then
    export $(cat .env | grep -v '#' | xargs)
else
    echo "Error: .env file not found"
    exit 1
fi

# Required environment variables check
if [ -z "$REMOTE_HOST" ] || [ -z "$REMOTE_USER" ] || [ -z "$SSH_KEY_PATH" ] || [ -z "$REMOTE_DIR" ]; then
    echo "Error: Required environment variables not set in .env file"
    echo "Please ensure REMOTE_HOST, REMOTE_USER, SSH_KEY_PATH, and REMOTE_DIR are set"
    exit 1
fi

# Check for scenarios file
if [ -z "$1" ]; then
    echo "Error: No scenarios file provided"
    echo "Usage: $0 scenarios.json [output_dir]"
    exit 1
fi

SCENARIOS_FILE="$1"
OUTPUT_DIR="${2:-behavior_trees}"

if [ ! -f "$SCENARIOS_FILE" ]; then
    echo "Error: Scenarios file '$SCENARIOS_FILE' not found"
    exit 1
fi

# SSH commands
SSH_CMD="ssh -i $SSH_KEY_PATH ${REMOTE_USER}@${REMOTE_HOST}"
SCP_CMD="scp -i $SSH_KEY_PATH"

# Function to verify SSH connection
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
submit_job() {
    if ! verify_ssh_connection; then
        exit 1
    fi
    
    echo "Creating necessary directories on remote server..."
    $SSH_CMD "mkdir -p ${REMOTE_DIR}/runs"
    
    echo "Copying files to remote server..."
    # Copy the Slurm script
    $SCP_CMD run_model.slurm ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    # Copy the scenarios file
    $SCP_CMD "$SCENARIOS_FILE" ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/scenarios.json
    # Copy Python scripts if they exist locally
    for script in demo_ssh.py retrieve_context.py upsert_document.py; do
        if [ -f "$script" ]; then
            $SCP_CMD "$script" ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
        fi
    done
    # Copy .env file if needed
    if [ -f ".env" ]; then
        $SCP_CMD .env ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    fi
    
    echo "Submitting SLURM job..."
    JOB_ID=$($SSH_CMD "cd ${REMOTE_DIR} && sbatch run_model.slurm")
    
    if [ $? -eq 0 ]; then
        echo "Job submitted successfully: $JOB_ID"
        
        # Wait for job completion
        echo "Waiting for job completion..."
        while true; do
            sleep 30
            JOB_STATE=$($SSH_CMD "squeue -j ${JOB_ID##* } -h -o %t" 2>/dev/null)
            if [ -z "$JOB_STATE" ]; then
                break
            fi
            echo "Job state: $JOB_STATE"
        done
        
        # Create local output directory
        mkdir -p "$OUTPUT_DIR"
        
        # Copy results back
        echo "Copying results back to local machine..."
        $SCP_CMD -r ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/runs/*/xml/* "$OUTPUT_DIR/"
        $SCP_CMD -r ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/runs/*/metadata/* "$OUTPUT_DIR/"
        
        echo "✅ Job completed and results copied to $OUTPUT_DIR"
    else
        echo "❌ Failed to submit job"
        exit 1
    fi
}

# Parse command line argument
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