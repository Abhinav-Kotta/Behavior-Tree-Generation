#!/bin/bash

# Create prompts file if provided
if [ $# -gt 0 ]; then
    echo "$@" > prompts.txt
fi

# Submit the job
sbatch run_model.slurm