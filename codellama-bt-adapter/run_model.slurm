#!/bin/bash
#SBATCH --job-name=behavior_tree_gen
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:1
#SBATCH --mem=32GB
#SBATCH --time=02:00:00
#SBATCH --error=job.%J.err
#SBATCH --output=job.%J.out

# Set paths
export BASE_MODEL_PATH="../models/codellama/CodeLlama-7b-Instruct-hf"
export LORA_ADAPTER_PATH="./codellama-bt-adapter"
export OUTPUT_DIR="outputs"

# Create job-specific directory
mkdir -p $SLURM_JOB_ID
export debug_logs="$SLURM_JOB_ID/job_$SLURM_JOB_ID.log"
export benchmark_logs="$SLURM_JOB_ID/job_$SLURM_JOB_ID.log"

# Load required modules
module purge
module load cuda/11.8
module load anaconda3

# Activate conda environment
source activate bt_env  # Replace with your environment name

# Enter working directory
cd $SLURM_SUBMIT_DIR

# Log job information
echo "JobID: $SLURM_JOB_ID" >> $debug_logs
echo "Running on $SLURM_NODELIST" >> $debug_logs
echo "Running on $SLURM_NNODES nodes." >> $debug_logs
echo "Running on $SLURM_NPROCS processors." >> $debug_logs
echo "Current working directory is `pwd`" >> $debug_logs
echo "Using GPU: $CUDA_VISIBLE_DEVICES" >> $debug_logs

# Module debugging
module list >> $debug_logs
date >> $benchmark_logs

# GPU debugging
nvidia-smi >> $debug_logs

# Check available memory
echo "Memory limits:" >> $benchmark_logs
ulimit -a >> $benchmark_logs

# Function to run model operations
run_model_operation() {
    local operation=$1
    local prompt_file=$2
    local output_file=$3
    
    echo "Running operation: $operation" >> $debug_logs
    date >> $benchmark_logs
    
    case $operation in
        "init")
            ./run_model.sh init
            ;;
        "generate")
            ./run_model.sh generate "$prompt_file" "$output_file"
            ;;
        "gpu-status")
            ./run_model.sh gpu-status
            ;;
        *)
            echo "Unknown operation: $operation" >> $debug_logs
            exit 1
            ;;
    esac
    
    date >> $benchmark_logs
}

# Ensure scripts are executable
chmod +x run_model.sh

# Initialize model and check GPU
echo "Checking GPU status..." >> $debug_logs
run_model_operation "gpu-status"

echo "Initializing model..." >> $debug_logs
run_model_operation "init"

# Process input prompts if provided
if [ -f "prompts.txt" ]; then
    echo "Processing prompts from file..." >> $debug_logs
    mkdir -p "${OUTPUT_DIR}/batch_${SLURM_JOB_ID}"
    
    while IFS= read -r prompt || [ -n "$prompt" ]; do
        timestamp=$(date +%Y%m%d_%H%M%S)
        prompt_file="${OUTPUT_DIR}/batch_${SLURM_JOB_ID}/prompt_${timestamp}.txt"
        output_file="${OUTPUT_DIR}/batch_${SLURM_JOB_ID}/response_${timestamp}.json"
        
        echo "$prompt" > "$prompt_file"
        echo "Processing prompt: $prompt" >> $debug_logs
        
        run_model_operation "generate" "$prompt_file" "$output_file"
    done < "prompts.txt"
else
    echo "No prompts.txt file found" >> $debug_logs
fi

# Collect GPU usage statistics
nvidia-smi --query-gpu=timestamp,name,utilization.gpu,memory.used,memory.total --format=csv >> "$SLURM_JOB_ID/gpu_stats.csv"

# Cleanup
mv job.$SLURM_JOB_ID.err $SLURM_JOB_ID/
mv job.$SLURM_JOB_ID.out $SLURM_JOB_ID/

echo "Job completed" >> $debug_logs
date >> $benchmark_logs