#!/bin/bash

# run_model.sh
# Place this in your bt_adapter directory on the GPU server

# Default paths - can be overridden by environment variables
export BASE_MODEL_PATH=${BASE_MODEL_PATH:-"../models/codellama/CodeLlama-7b-Instruct-hf"}
export LORA_ADAPTER_PATH=${LORA_ADAPTER_PATH:-"./"}
export OUTPUT_DIR=${OUTPUT_DIR:-"./outputs"}
export PYTHONPATH="${PYTHONPATH}:$(pwd)"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Function to initialize the model using demo.py functions
initialize_model() {
    echo "Initializing model..."
    python3 -c "
from demo import initialize_model
import json
import os

try:
    model, tokenizer = initialize_model('${BASE_MODEL_PATH}', '${LORA_ADAPTER_PATH}')
    
    # Save initialization status
    with open('${OUTPUT_DIR}/init_status.json', 'w') as f:
        json.dump({
            'status': 'success',
            'device': str(next(model.parameters()).device),
            'gpu_available': str(model.device)
        }, f)
    
    print('Model initialized successfully')
    
except Exception as e:
    with open('${OUTPUT_DIR}/init_status.json', 'w') as f:
        json.dump({
            'status': 'error',
            'error': str(e)
        }, f)
    print(f'Error initializing model: {str(e)}')
    exit(1)
"
}

# Function to generate response using demo.py functions
generate_response() {
    local prompt_file=$1
    local output_file=$2
    
    echo "Generating response..."
    python3 -c "
from demo import generate_and_save_response, initialize_model
import json
import os

try:
    # Load prompt
    with open('${prompt_file}', 'r') as f:
        prompt = f.read()
    
    # Initialize model and tokenizer
    model, tokenizer = initialize_model('${BASE_MODEL_PATH}', '${LORA_ADAPTER_PATH}')
    
    # Generate response
    response = generate_and_save_response(
        model,
        tokenizer,
        prompt,
        '${OUTPUT_DIR}/generation_log.txt',
        max_new_tokens=512,
        temperature=0.7
    )
    
    # Save response
    with open('${output_file}', 'w') as f:
        json.dump({
            'status': 'success',
            'response': response
        }, f)
    
except Exception as e:
    with open('${output_file}', 'w') as f:
        json.dump({
            'status': 'error',
            'error': str(e)
        }, f)
    print(f'Error generating response: {str(e)}')
    exit(1)
"
}

# Function to check GPU status
check_gpu() {
    echo "Checking GPU status..."
    python3 -c "
import torch
import json

try:
    gpu_info = {
        'gpu_available': torch.cuda.is_available(),
        'gpu_count': torch.cuda.device_count() if torch.cuda.is_available() else 0,
        'gpu_name': torch.cuda.get_device_name(0) if torch.cuda.is_available() else None,
        'cuda_version': torch.version.cuda if torch.cuda.is_available() else None
    }
    
    with open('${OUTPUT_DIR}/gpu_status.json', 'w') as f:
        json.dump(gpu_info, f)
    
    print(f'GPU Status: {gpu_info}')
    
except Exception as e:
    print(f'Error checking GPU: {str(e)}')
    exit(1)
"
    nvidia-smi >> "${OUTPUT_DIR}/gpu_status.txt"
}

# Main execution
case "$1" in
    "init")
        check_gpu
        initialize_model
        ;;
    "generate")
        if [ "$#" -ne 3 ]; then
            echo "Usage: $0 generate <prompt_file> <output_file>"
            exit 1
        fi
        generate_response "$2" "$3"
        ;;
    "gpu-status")
        check_gpu
        ;;
    *)
        echo "Usage: $0 {init|generate|gpu-status}"
        echo "  init - Initialize the model"
        echo "  generate <prompt_file> <output_file> - Generate response"
        echo "  gpu-status - Check GPU status"
        exit 1
        ;;
esac