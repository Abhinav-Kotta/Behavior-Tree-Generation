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
    # Copy demo_ssh.py if it exists locally
    if [ -f "demo_ssh.py" ]; then
        $SCP_CMD demo_ssh.py ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    fi
    # Copy prompts.txt if it exists
    if [ -f "prompts.txt" ]; then
        $SCP_CMD prompts.txt ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    fi
    # Copy .env file if needed
    if [ -f ".env" ]; then
        $SCP_CMD .env ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/
    fi
    
    echo "Submitting SLURM job..."
    $SSH_CMD "cd ${REMOTE_DIR} && sbatch run_model.slurm"
}

# Parse command line argument
case "$1" in
    -v|--verify)
        verify_ssh_connection
        exit
        ;;
    "")
        submit_job
        ;;
    *)
        echo "Usage: $0 [-v|--verify]"
        echo "  -v, --verify    Verify SSH connection only"
        echo "  (no flag)       Submit SLURM job"
        exit 1
        ;;
esac