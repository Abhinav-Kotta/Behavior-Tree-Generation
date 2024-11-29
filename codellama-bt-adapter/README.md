This directory contains the core functionality of the codellama adapter and RAG. Here are some of the relevant files.
## demo_ssh.py
demo_ssh.py is a Python script that handles the generation of military behavior trees using a fine-tuned large language model with LoRA adapters. It integrates with a retrieval-augmented generation (RAG) system to provide context-aware behavior tree generation.
### Dependencies
1. **torch**: PyTorch for model operations
2. **transformers**: Hugging Face transformers library
3. **peft**: Paramter-Efficient-Fine-Tuning library
4. **xml.dom.minidom**: XML parsing and formatting
5. **logging**: logging functionality
6. **dotenv**: environemtn variable management
7. **retrieve_context**: custom module for RAG operations \

### Main Functions
- **initialize_model(base_model_path, lora_adapter_path)**: Loads and initializes the language model and tokenizer with LoRA adapters.
- **extract_xml_from_response(response)**: Extracts and formats XML content from the model's output.
- **generate_behavior_tree(model, tokenizer, prompt, scenario_name, output_dir)**: Generates a behavior tree based on a given prompt and saves it as XML.

### Usage
```python3 demo_ssh.py scenarios.json output_dir```

### Input/Output
#### Features
- Context-aware behavior tree generation
- Military doctrine integration through RAG
- XML output with proper formatting
- Metadata generation for each tree
- GPU acceleration support
- Comprehensive logging

#### Requirements
- Python 3.8+
- PyTorch
- Transformers
- PEFT
- Pinecone
- dotenv

## codellama_test.py
A testing script for validating CodeLlama model performance with LoRA adapters for behavior tree generation. Tests model initialization, adapter functionality, and batch prompt processing.
### Usage
```python3 codellama_test.py```
### Features
- Model initialization testing
- LoRA adapter verification
- Test prompt processing
- Detailed logging of outputs
- GPU acceleration when available
### Environment Variables
Required in `.env`:
- **BASE_MODEL_PATH**: Path to CodeLlama model
- **LORA_ADAPTER_PATH**: Path to fine-tuned LoRA weights
### Output
Creates a timestamped output file in your GPU cluster account containing:

- Model configuration details
- LoRA verification results
- Generated responses