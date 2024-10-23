from retrieve_context import retrieve_similar_content
from codellama_test import (
    generate_and_save_response,
    save_to_file,
    AutoModelForCausalLM,
    AutoTokenizer,
    PeftModel
)
import datetime
import os
import torch

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

def run_demo():
    # Configuration
    BASE_MODEL_PATH = "/lustre/fs1/home/akotta/Behavior-Tree-Generation/models--codellama--CodeLlama-7b-Instruct-hf/snapshots/22cb240e0292b0b5ab4c17ccd97aa3a2f799cbed"
    LORA_ADAPTER_PATH = "/lustre/fs1/home/akotta/Behavior-Tree-Generation/codellama-bt-adapter"
    PINECONE_INDEX = "pdf-rag-index"
    OUTPUT_DIR = "outputs"
    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    OUTPUT_FILE = f"{OUTPUT_DIR}/demo_generation_{timestamp}.txt"
    
    try:
        # Initialize model
        model, tokenizer = initialize_model(BASE_MODEL_PATH, LORA_ADAPTER_PATH)
        
        # Log configuration
        config_info = f"""
        === Demo Configuration ===
        Base Model: {BASE_MODEL_PATH}
        LoRA Adapter: {LORA_ADAPTER_PATH}
        Pinecone Index: {PINECONE_INDEX}
        Output File: {OUTPUT_FILE}
        Start Time: {datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
        {'='*50}
        """
        save_to_file(config_info, OUTPUT_FILE, mode='w')
        
        while True:
            print("\nEnter your behavior tree query (or 'quit' to exit):")
            query = input().strip()
            
            if query.lower() == 'quit':
                break
            
            # Get context from Pinecone
            print("Retrieving relevant context...")
            results = retrieve_similar_content(query, PINECONE_INDEX, top_k=3)
            
            # Extract context from results
            context = ""
            for match in results.matches:
                if hasattr(match, 'metadata') and 'text' in match.metadata:
                    context += match.metadata['text'] + "\n\n"
            
            # Log retrieved context
            context_info = f"""
            === Retrieved Context ===
            Query: {query}
            Number of matches: {len(results.matches)}
            Context length: {len(context)} characters

            Context:
            {context}
            {'='*50}
            """
            save_to_file(context_info, OUTPUT_FILE)
            
            # Create enhanced prompt with context
            enhanced_prompt = f"""[INST] Using the following context about military behavior trees:

            {context}

            Generate a behavior tree in XML format for the following scenario:
            {query} [/INST]"""
            
            # Generate behavior tree
            print("Generating behavior tree...")
            response = generate_and_save_response(
                model,
                tokenizer,
                enhanced_prompt,
                OUTPUT_FILE,
                max_new_tokens=512,
                temperature=0.7
            )
            
            print("\nGenerated Behavior Tree:")
            print(response)
            print(f"\nOutput saved to: {OUTPUT_FILE}")
        
    except Exception as e:
        error_msg = f"\nAn error occurred: {str(e)}"
        print(error_msg)
        save_to_file(error_msg, OUTPUT_FILE)

if __name__ == "__main__":
    run_demo()