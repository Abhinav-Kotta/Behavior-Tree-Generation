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
export PYTHONPATH="${PYTHONPATH}:$(pwd)"

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

# Run demo.py with the prompts file if it exists
if [ -f "prompts.txt" ]; then
    echo "Running demo.py in batch mode with prompts.txt" >> $debug_logs
    python3 demo_ssh.py prompts.txt >> $debug_logs 2>&1
else
    echo "No prompts.txt found, running demo.py in interactive mode" >> $debug_logs
    python3 demo_ssh.py >> $debug_logs 2>&1
fi

# Collect GPU usage statistics
nvidia-smi --query-gpu=timestamp,name,utilization.gpu,memory.used,memory.total --format=csv >> "$SLURM_JOB_ID/gpu_stats.csv"

# Cleanup
mv job.$SLURM_JOB_ID.err $SLURM_JOB_ID/
mv job.$SLURM_JOB_ID.out $SLURM_JOB_ID/

echo "Job completed" >> $debug_logs
date >> $benchmark_logs