import torch
from transformers import AutoModelForCausalLM, AutoTokenizer
from peft import PeftModel, PeftConfig
import datetime
import os

def save_to_file(content, filename, mode='a'):
    """
    Save content to a file, creating directory if it doesn't exist
    """
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    
    with open(filename, mode, encoding='utf-8') as f:
        f.write(content + '\n')

def verify_lora_adapters(model, tokenizer, output_file):
    """Verify that LoRA adapters are properly loaded and active"""
    
    verification_info = "\n=== LoRA Adapter Verification ===\n"
    
    # Check if model is a PeftModel
    verification_info += f"Is PeftModel: {isinstance(model, PeftModel)}\n"
    
    # Check active adapters
    if hasattr(model, 'active_adapters'):
        verification_info += f"Active adapters: {model.active_adapters}\n"
    
    # Check available adapter modules
    if hasattr(model, 'peft_config'):
        verification_info += "\nPEFT Configuration:\n"
        for adapter_name, config in model.peft_config.items():
            verification_info += f"Adapter name: {adapter_name}\n"
            verification_info += f"LoRA rank: {config.r}\n"
            verification_info += f"LoRA alpha: {config.lora_alpha}\n"
            verification_info += f"Target modules: {config.target_modules}\n"
    
    # Test output difference
    verification_info += "\nTesting output difference with and without adapter...\n"
    test_prompt = "[INST] Write a short military command [/INST]"
    
    # Generate with adapter
    inputs = tokenizer(test_prompt, return_tensors="pt").to(model.device)
    with torch.no_grad():
        output_with_adapter = model.generate(
            **inputs,
            max_new_tokens=50,
            temperature=0.7,
            top_p=0.95,
        )
    response_with_adapter = tokenizer.decode(output_with_adapter[0], skip_special_tokens=True)
    
    # Temporarily disable adapter
    model.disable_adapter_layers()
    with torch.no_grad():
        output_without_adapter = model.generate(
            **inputs,
            max_new_tokens=50,
            temperature=0.7,
            top_p=0.95,
        )
    response_without_adapter = tokenizer.decode(output_without_adapter[0], skip_special_tokens=True)
    model.enable_adapter_layers()  # Re-enable adapter
    
    verification_info += "\nOutput with adapter:\n"
    verification_info += response_with_adapter
    verification_info += "\n\nOutput without adapter:\n"
    verification_info += response_without_adapter
    verification_info += "\n" + "="*50 + "\n"
    
    save_to_file(verification_info, output_file)

    print(verification_info)
    
    return True

def generate_and_save_response(
    model,
    tokenizer,
    prompt,
    output_file,
    max_new_tokens=512,
    temperature=0.7,
    top_p=0.95,
):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    inputs = tokenizer(prompt, return_tensors="pt").to(model.device)
    
    with torch.no_grad():
        outputs = model.generate(
            **inputs,
            max_new_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
            pad_token_id=tokenizer.eos_token_id
        )
    
    response = tokenizer.decode(outputs[0], skip_special_tokens=True)
    response = response[len(tokenizer.decode(inputs.input_ids[0], skip_special_tokens=True)):]
    
    output_content = f"""
    === Generation Time: {timestamp} ===
    Prompt: {prompt}
    Temperature: {temperature}
    Top_p: {top_p}
    Max new tokens: {max_new_tokens}

    Response:
    {response}

    {'='*50}
    """
    
    save_to_file(output_content, output_file)
    return response

def initialize_model(base_model_path, lora_adapter_path):
    """Initialize the model and tokenizer with LoRA adapter"""
    print("Loading model...")
    model = AutoModelForCausalLM.from_pretrained(
        base_model_path,
        torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
        device_map="auto" if torch.cuda.is_available() else None,
        trust_remote_code=True
    )
    
    print("Loading tokenizer...")
    tokenizer = AutoTokenizer.from_pretrained(
        base_model_path,
        trust_remote_code=True
    )
    tokenizer.pad_token = tokenizer.eos_token
    
    print("Loading LoRA adapter...")
    model = PeftModel.from_pretrained(
        model,
        lora_adapter_path,
        torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
    )
    
    if torch.cuda.is_available():
        model.half()
    
    model.eval()
    return model, tokenizer

if __name__ == "__main__":
    BASE_MODEL_PATH = "/lustre/fs1/home/akotta/Behavior-Tree-Generation/models--codellama--CodeLlama-7b-Instruct-hf/snapshots/22cb240e0292b0b5ab4c17ccd97aa3a2f799cbed"
    LORA_ADAPTER_PATH = "/lustre/fs1/home/akotta/Behavior-Tree-Generation/codellama-bt-adapter"
    
    OUTPUT_DIR = "outputs"
    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    OUTPUT_FILE = f"{OUTPUT_DIR}/generation_output_{timestamp}.txt"
    
    try:
        model, tokenizer = initialize_model(BASE_MODEL_PATH, LORA_ADAPTER_PATH)
        
        # Save model loading information
        model_info = f"""
        === Model Configuration ===
        Base Model: {BASE_MODEL_PATH}
        LoRA Adapter: {LORA_ADAPTER_PATH}
        Device: {'CUDA' if torch.cuda.is_available() else 'CPU'}
        Output File: {OUTPUT_FILE}
        Loading Time: {datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
        {'='*50}
        """
        save_to_file(model_info, OUTPUT_FILE, mode='w')
        
        # Verify LoRA adapters
        print("\nVerifying LoRA adapters...")
        verify_lora_adapters(model, tokenizer, OUTPUT_FILE)
        
        # Example prompts - you can add more
        prompts = [
            "[INST] Create an example behavior tree for a military scenario in XML[/INST]",
            "[INST] Create an example behavior tree for a reconnaissance mission in XML[/INST]",
            "[INST] Generate an example behavior tree for defensive operations in XML[/INST]"
        ]
        
        for i, prompt in enumerate(prompts, 1):
            print(f"\nGenerating response for prompt {i}/{len(prompts)}...")
            response = generate_and_save_response(
                model,
                tokenizer,
                prompt,
                OUTPUT_FILE,
                max_new_tokens=512,
                temperature=0.7
            )
            print(f"Response {i} generated and saved to {OUTPUT_FILE}")
        
        print(f"\nAll responses have been saved to: {OUTPUT_FILE}")
        
    except Exception as e:
        error_msg = f"\nAn error occurred: {str(e)}"
        print(error_msg)
        save_to_file(error_msg, OUTPUT_FILE)
