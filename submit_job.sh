#!/bin/bash

# submit_job.sh - SSH Key Version

# Load environment variables from .env file
if [ -f .env ]; then
    export $(cat .env | grep -v '#' | xargs)
else
    echo "Error: .env file not found"
    exit 1
fi

# Required environment variables
if [ -z "$REMOTE_HOST" ] || [ -z "$REMOTE_USER" ] || [ -z "$REMOTE_DIR" ] || [ -z "$SSH_KEY_PATH" ]; then
    echo "Error: Required environment variables not set in .env file"
    echo "Please ensure REMOTE_HOST, REMOTE_USER, REMOTE_DIR, and SSH_KEY_PATH are set"
    exit 1
fi

# SSH command with key
SSH_CMD="ssh -i $SSH_KEY_PATH ${REMOTE_USER}@${REMOTE_HOST}"
SCP_CMD="scp -i $SSH_KEY_PATH"

# Function to verify SSH connection
verify_ssh_connection() {
    echo "Verifying SSH connection to ${REMOTE_HOST}..."
    
    # Try to execute a simple command on the remote host
    if $SSH_CMD "echo 'SSH connection successful' && exit" >/dev/null 2>&1; then
        echo "✅ SSH connection verified successfully"
        return 0
    else
        echo "❌ Failed to establish SSH connection"
        echo "Please check:"
        echo "  1. SSH key path: $SSH_KEY_PATH"
        echo "  2. Remote host: $REMOTE_HOST"
        echo "  3. Username: $REMOTE_USER"
        echo "  4. SSH key permissions (should be 600)"
        echo "  5. Network connectivity"
        return 1
    fi
}

# Function to show script usage
show_usage() {
    echo "Usage: $0 [options] [prompt...]"
    echo "Options:"
    echo "  -h, --help        Show this help message"
    echo "  -f, --file FILE   Use prompts from file"
    echo "  -s, --status      Check status of running jobs"
    echo "  -l, --logs JOB_ID Get logs for specific job"
    echo "  -c, --check       Check remote GPU status"
    echo "  -v, --verify      Verify SSH connection only"
    exit 1
}

# Function to check job status
check_job_status() {
    $SSH_CMD "squeue -u ${REMOTE_USER}"
}

# Function to get job logs
get_job_logs() {
    if ! verify_ssh_connection; then
        exit 1
    fi
    local job_id=$1
    
    # Check both job logs and SLURM logs
    echo "=== Job Output Logs ==="
    $SSH_CMD "cat ${REMOTE_DIR}/logs/job_logs/job_${job_id}.log 2>/dev/null || echo 'Job log not found'"
    echo -e "\n=== SLURM Logs ==="
    $SSH_CMD "cat ${REMOTE_DIR}/logs/slurm_logs/slurm_${job_id}.out 2>/dev/null || echo 'SLURM output log not found'"
    $SSH_CMD "cat ${REMOTE_DIR}/logs/slurm_logs/slurm_${job_id}.err 2>/dev/null || echo 'SLURM error log not found'"
}

# Function to check GPU status
check_gpu_status() {
    echo "Checking GPU status on ${REMOTE_HOST}..."
    $SSH_CMD "nvidia-smi"
}

# Function to submit job
submit_job() {
    echo "Preparing to submit job to ${REMOTE_HOST}..."
    
    # Create remote directory if it doesn't exist
    echo "Ensuring remote directory exists..."
    $SSH_CMD "mkdir -p ${REMOTE_DIR}/codellama-bt-adapter"
    
    # Copy necessary files
    echo "Copying files to remote server..."
    $SCP_CMD -r codellama-bt-adapter/* ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/codellama-bt-adapter/
    $SCP_CMD run_model.slurm ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    
    if [ -f "prompts.txt" ]; then
        echo "Copying prompts file..."
        $SCP_CMD prompts.txt ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    fi
    
    # Submit the job
    echo "Submitting SLURM job..."
    job_info=$($SSH_CMD "cd ${REMOTE_DIR} && sbatch run_model.slurm")
    
    if [ $? -eq 0 ]; then
        # Extract job ID and save it
        job_id=$(echo $job_info | grep -oP '\d+')
        echo $job_id > .last_job_id
        
        echo "Job submitted successfully!"
        echo "Job ID: $job_id"
        echo "Monitor your job with: $0 -l $job_id"
        echo "Check job status with: $0 -s"
    else
        echo "Error submitting job"
        exit 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            ;;
        -f|--file)
            if [ -f "$2" ]; then
                cp "$2" prompts.txt
                shift 2
            else
                echo "Error: File $2 not found"
                exit 1
            fi
            ;;
        -s|--status)
            check_job_status
            exit 0
            ;;
        -l|--logs)
            if [ -n "$2" ]; then
                get_job_logs "$2"
                exit 0
            else
                echo "Error: Please provide a job ID"
                exit 1
            fi
            ;;
        -c|--check)
            check_gpu_status
            exit 0
            ;;
        -v|--verify)
            verify_ssh_connection
            exit 0
            ;;
        *)
            # Collect remaining arguments as prompts
            echo "$@" > prompts.txt
            break
            ;;
    esac
done

# Submit the job if no other action was taken
submit_job