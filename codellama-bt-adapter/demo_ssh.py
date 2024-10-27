import torch
from transformers import AutoModelForCausalLM, AutoTokenizer
from peft import PeftModel
import datetime
import os
import logging
from pathlib import Path
from retrieve_context import retrieve_similar_content
from dotenv import load_dotenv
import xml.dom.minidom
import json

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
            model.half()
        
        model.eval()
        logger.info("Model initialization complete")
        return model, tokenizer

    except Exception as e:
        logger.error(f"Error initializing model: {str(e)}")
        raise

def extract_xml_from_response(response):
    """Extract and format XML content from the model's response"""
    try:
        # Find XML content between tags if present
        start_idx = response.find('<?xml')
        if start_idx == -1:
            start_idx = response.find('<behavior')
        if start_idx == -1:
            start_idx = response.find('<root')
            
        if start_idx == -1:
            return response  # Return original if no XML found
            
        end_idx = response.rfind('>')
        if end_idx == -1:
            return response
            
        xml_content = response[start_idx:end_idx+1]
        
        # Try to format XML
        try:
            dom = xml.dom.minidom.parseString(xml_content)
            pretty_xml = dom.toprettyxml(indent="  ")
            return pretty_xml
        except:
            return xml_content
            
    except Exception as e:
        logger.error(f"Error extracting XML: {str(e)}")
        return response

def generate_behavior_tree(
    model,
    tokenizer,
    prompt,
    scenario_name,
    output_dir,
    max_new_tokens=512,
    temperature=0.7,
    top_p=0.95,
):
    """Generate a behavior tree and save as XML"""
    try:
        # Get context from Pinecone
        results = retrieve_similar_content(prompt, "pdf-rag-index", top_k=3)
        
        context = "\n\n".join([
            match.metadata['text']
            for match in results.matches
            if hasattr(match, 'metadata') and 'text' in match.metadata
        ])
        
        # Create enhanced prompt
        enhanced_prompt = f"""[INST] Using the following context about military behavior trees:

        {context}

        Generate a behavior tree in XML format for the following scenario:
        {prompt} [/INST]"""
        
        # Generate response
        inputs = tokenizer(enhanced_prompt, return_tensors="pt").to(model.device)
        
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
        
        # Extract and format XML
        xml_content = extract_xml_from_response(response)
        
        # Save XML file
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        xml_filename = f"{scenario_name}_{timestamp}.xml"
        xml_path = os.path.join(output_dir, "xml", xml_filename)
        os.makedirs(os.path.dirname(xml_path), exist_ok=True)
        
        with open(xml_path, 'w', encoding='utf-8') as f:
            f.write(xml_content)
        
        # Save metadata
        metadata = {
            "timestamp": timestamp,
            "scenario": scenario_name,
            "prompt": prompt,
            "context_chunks": len(results.matches),
            "generation_params": {
                "max_new_tokens": max_new_tokens,
                "temperature": temperature,
                "top_p": top_p
            }
        }
        
        metadata_path = os.path.join(output_dir, "metadata", f"{scenario_name}_{timestamp}.json")
        os.makedirs(os.path.dirname(metadata_path), exist_ok=True)
        
        with open(metadata_path, 'w', encoding='utf-8') as f:
            json.dump(metadata, f, indent=2)
            
        logger.info(f"Generated behavior tree saved to {xml_path}")
        return xml_path

    except Exception as e:
        logger.error(f"Error generating behavior tree: {str(e)}")
        raise

def process_scenarios(scenarios_file, output_dir):
    """Process multiple scenarios from a JSON file"""
    load_dotenv()
    
    try:
        # Load scenarios
        with open(scenarios_file, 'r') as f:
            scenarios = json.load(f)
            
        # Initialize model
        model, tokenizer = initialize_model(
            os.getenv('BASE_MODEL_PATH'),
            os.getenv('LORA_ADAPTER_PATH')
        )
        
        # Create output directories
        os.makedirs(output_dir, exist_ok=True)
        os.makedirs(os.path.join(output_dir, "xml"), exist_ok=True)
        os.makedirs(os.path.join(output_dir, "metadata"), exist_ok=True)
        
        # Process each scenario
        generated_files = []
        for scenario in scenarios:
            name = scenario.get('name', 'unnamed_scenario')
            prompt = scenario.get('prompt', '')
            
            if not prompt:
                logger.warning(f"Skipping scenario '{name}': no prompt provided")
                continue
                
            try:
                xml_path = generate_behavior_tree(
                    model,
                    tokenizer,
                    prompt,
                    name,
                    output_dir
                )
                generated_files.append(xml_path)
                
            except Exception as e:
                logger.error(f"Error processing scenario '{name}': {str(e)}")
                continue
        
        return generated_files
        
    except Exception as e:
        logger.error(f"Fatal error: {str(e)}")
        raise

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) != 3:
        print("Usage: python demo_ssh.py scenarios.json output_dir")
        sys.exit(1)
        
    scenarios_file = sys.argv[1]
    output_dir = sys.argv[2]
    
    process_scenarios(scenarios_file, output_dir)