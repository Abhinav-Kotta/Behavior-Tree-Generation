#!/bin/bash
#SBATCH --job-name=behavior_tree_gen
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:1
#SBATCH --mem=32GB
#SBATCH --time=02:00:00
#SBATCH --error=runs/%j/job.err
#SBATCH --output=runs/%j/job.out

# Exit on error
set -e

# Define base directory (where the script is located)
export BASE_DIR="/lustre/fs1/home/akotta/Behavior-Tree-Generation"
export SCRIPT_DIR="${BASE_DIR}/codellama-bt-adapter"

# Define custom output directory and create necessary subdirectories
export RUN_DIR="${SCRIPT_DIR}/runs/${SLURM_JOB_ID}"
mkdir -p "${RUN_DIR}"/{logs,outputs}

# Set environment variables
export BASE_MODEL_PATH="${BASE_DIR}/models/codellama/models--codellama--CodeLlama-7b-Instruct-hf/snapshots/22cb240e0292b0b5ab4c17ccd97aa3a2f799cbed"
export LORA_ADAPTER_PATH="${SCRIPT_DIR}"
export OUTPUT_DIR="${RUN_DIR}/outputs"
export PYTHONPATH="${PYTHONPATH}:${SCRIPT_DIR}"

# Define log files
export DEBUG_LOG="${RUN_DIR}/logs/debug.log"
export BENCHMARK_LOG="${RUN_DIR}/logs/benchmark.log"
export GPU_STATS="${RUN_DIR}/logs/gpu_stats.csv"

# Function to log messages with timestamps
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "${DEBUG_LOG}"
}

# Function to cleanup background processes
cleanup() {
    local exit_code=$?
    log_message "Cleaning up background processes..."
    
    # Kill GPU monitoring if it's running
    if [ ! -z "$GPU_MONITOR_PID" ]; then
        if kill -0 $GPU_MONITOR_PID 2>/dev/null; then
            kill $GPU_MONITOR_PID 2>/dev/null || true
            wait $GPU_MONITOR_PID 2>/dev/null || true
        fi
    fi
    
    log_message "Cleanup completed with exit code: $exit_code"
    exit $exit_code
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Initialize log files
log_message "=== Job Started ==="
log_message "JobID: ${SLURM_JOB_ID}"
log_message "Running on ${SLURM_NODELIST}"
log_message "Base directory: ${BASE_DIR}"
log_message "Script directory: ${SCRIPT_DIR}"

# Load required modules
log_message "Loading modules..."
module purge
module load anaconda/anaconda-2023.09
log_message "Loaded modules:"
module list >> "${DEBUG_LOG}"

# Activate conda environment
log_message "Activating conda environment..."
source activate /home/akotta/.conda/envs/lora_test

# Change to script directory
log_message "Changing to script directory..."
cd "${SCRIPT_DIR}"
log_message "Current working directory: $(pwd)"

# Log system information
log_message "=== System Information ==="
nvidia-smi >> "${DEBUG_LOG}"
echo "Memory limits:" >> "${BENCHMARK_LOG}"
ulimit -a >> "${BENCHMARK_LOG}"

# Start GPU monitoring in background with proper error handling
log_message "Starting GPU monitoring..."
(
    # Create header for GPU stats CSV if it doesn't exist
    if [ ! -f "${GPU_STATS}" ]; then
        echo "timestamp,gpu_name,gpu_utilization,memory_used,memory_total" > "${GPU_STATS}"
    fi
    
    while true; do
        if ! nvidia-smi --query-gpu=timestamp,name,utilization.gpu,memory.used,memory.total --format=csv,noheader >> "${GPU_STATS}"; then
            log_message "Warning: Failed to collect GPU stats"
            sleep 30
            continue
        fi
        sleep 30
    done
) &
GPU_MONITOR_PID=$!
disown $GPU_MONITOR_PID

# Run the main script
log_message "=== Starting Main Script ==="
if [ -f "prompts.txt" ]; then
    log_message "Running demo_ssh.py in batch mode with prompts.txt"
    python3 demo_ssh.py prompts.txt 2>&1 | tee -a "${DEBUG_LOG}"
else
    log_message "No prompts.txt found, running demo_ssh.py in interactive mode"
    python3 demo_ssh.py 2>&1 | tee -a "${DEBUG_LOG}"
fi

# Log completion
log_message "=== Job Completed ==="
date >> "${BENCHMARK_LOG}"

# Archive logs
tar -czf "${RUN_DIR}/logs.tar.gz" -C "${RUN_DIR}" logs/

# Cleanup will be handled by the trap