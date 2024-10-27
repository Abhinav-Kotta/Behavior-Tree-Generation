import torch
from transformers import AutoModelForCausalLM, AutoTokenizer
from peft import PeftModel
import datetime
import os
import json
import logging
from pathlib import Path
from retrieve_context import retrieve_similar_content
from dotenv import load_dotenv

# Set up logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

def initialize_model(base_model_path, lora_adapter_path):
    """Initialize the model and tokenizer with LoRA adapter"""
    try:
        logger.info(f"CUDA available: {torch.cuda.is_available()}")
        if torch.cuda.is_available():
            logger.info(f"GPU Device: {torch.cuda.get_device_name(0)}")
        
        logger.info("Loading tokenizer...")
        tokenizer = AutoTokenizer.from_pretrained(
            base_model_path,
            trust_remote_code=True
        )
        tokenizer.pad_token = tokenizer.eos_token
        
        logger.info("Loading model...")
        model = AutoModelForCausalLM.from_pretrained(
            base_model_path,
            torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
            device_map="auto" if torch.cuda.is_available() else None,
            trust_remote_code=True
        )
        
        logger.info("Loading LoRA adapter...")
        model = PeftModel.from_pretrained(
            model,
            lora_adapter_path,
            torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
        )
        
        if torch.cuda.is_available():
            model.half()  # Convert to FP16 for efficiency
        
        model.eval()
        logger.info("Model initialization complete")
        return model, tokenizer

    except Exception as e:
        logger.error(f"Error initializing model: {str(e)}")
        raise

def generate_and_save_response(
    model,
    tokenizer,
    prompt,
    output_file,
    max_new_tokens=512,
    temperature=0.7,
    top_p=0.95,
):
    """Generate response and save to file"""
    try:
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        logger.info("Generating response...")
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
        
        # Log generation metadata
        generation_info = {
            "timestamp": timestamp,
            "device": str(model.device),
            "max_new_tokens": max_new_tokens,
            "temperature": temperature,
            "top_p": top_p,
        }
        
        output_content = {
            "metadata": generation_info,
            "prompt": prompt,
            "response": response
        }
        
        # Save to file
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(output_content, f, indent=2)
        
        logger.info(f"Response saved to {output_file}")
        return response

    except Exception as e:
        logger.error(f"Error generating response: {str(e)}")
        raise

def run_demo(input_file=None, output_dir=None):
    """Main demo function that can handle both interactive and batch processing"""
    load_dotenv()

    # Configuration
    BASE_MODEL_PATH = os.getenv('BASE_MODEL_PATH')
    LORA_ADAPTER_PATH = os.getenv('LORA_ADAPTER_PATH')
    OUTPUT_DIR = output_dir or "outputs"
    
    try:
        # Initialize model
        model, tokenizer = initialize_model(BASE_MODEL_PATH, LORA_ADAPTER_PATH)
        
        # Batch processing mode
        if input_file and os.path.exists(input_file):
            logger.info(f"Processing prompts from {input_file}")
            with open(input_file, 'r') as f:
                prompts = f.readlines()
            
            for i, query in enumerate(prompts, 1):
                query = query.strip()
                if not query:
                    continue
                
                try:
                    logger.info(f"Processing prompt {i}/{len(prompts)}")
                    
                    # Get context from Pinecone
                    results = retrieve_similar_content(query, "pdf-rag-index", top_k=3)
                    
                    context = "\n\n".join([
                        match.metadata['text']
                        for match in results.matches
                        if hasattr(match, 'metadata') and 'text' in match.metadata
                    ])
                    
                    # Create enhanced prompt
                    enhanced_prompt = f"""[INST] Using the following context about military behavior trees:

                    {context}

                    Generate a behavior tree in XML format for the following scenario:
                    {query} [/INST]"""
                    
                    # Generate response
                    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
                    output_file = os.path.join(OUTPUT_DIR, f"generation_{timestamp}.json")
                    
                    response = generate_and_save_response(
                        model,
                        tokenizer,
                        enhanced_prompt,
                        output_file
                    )
                    
                    logger.info(f"Generated response saved to {output_file}")
                    
                except Exception as e:
                    logger.error(f"Error processing prompt {i}: {str(e)}")
                    continue
        
        # Interactive mode
        else:
            logger.info("Starting interactive mode")
            while True:
                print("\nEnter your behavior tree query (or 'quit' to exit):")
                query = input().strip()
                
                if query.lower() == 'quit':
                    break
                
                try:
                    # Get context from Pinecone
                    results = retrieve_similar_content(query, "pdf-rag-index", top_k=3)
                    
                    context = "\n\n".join([
                        match.metadata['text']
                        for match in results.matches
                        if hasattr(match, 'metadata') and 'text' in match.metadata
                    ])
                    
                    # Create enhanced prompt
                    enhanced_prompt = f"""[INST] Using the following context about military behavior trees:

                    {context}

                    Generate a behavior tree in XML format for the following scenario:
                    {query} [/INST]"""
                    
                    # Generate response
                    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
                    output_file = os.path.join(OUTPUT_DIR, f"generation_{timestamp}.json")
                    
                    response = generate_and_save_response(
                        model,
                        tokenizer,
                        enhanced_prompt,
                        output_file
                    )
                    
                    print("\nGenerated Behavior Tree:")
                    print(response)
                    print(f"\nOutput saved to: {output_file}")
                    
                except Exception as e:
                    logger.error(f"Error during generation: {str(e)}")
                    print(f"\nAn error occurred: {str(e)}")
                    continue
                    
    except Exception as e:
        logger.error(f"Fatal error: {str(e)}")
        raise

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        # Batch mode with input file
        run_demo(input_file=sys.argv[1])
    else:
        # Interactive mode
        run_demo()