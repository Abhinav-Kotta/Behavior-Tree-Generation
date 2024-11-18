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
            logger.warning("No XML tags found in response")
            return response  # Return original if no XML found
            
        end_idx = response.rfind('>')
        if end_idx == -1:
            logger.warning("No closing tag found in response")
            return response
            
        xml_content = response[start_idx:end_idx+1]
        
        # Try to format XML
        try:
            dom = xml.dom.minidom.parseString(xml_content)
            pretty_xml = dom.toprettyxml(indent="  ")
            # Remove blank lines
            pretty_xml = '\n'.join([line for line in pretty_xml.split('\n') if line.strip()])
            return pretty_xml
        except Exception as e:
            logger.warning(f"Failed to format XML: {str(e)}")
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
        logger.info(f"Generating behavior tree for scenario: {scenario_name}")
        
        # Get context from Pinecone
        results = retrieve_similar_content(prompt, "pdf-rag-index", top_k=3)
        
        context = "\n\n".join([
            match.metadata['text']
            for match in results.matches
            if hasattr(match, 'metadata') and 'text' in match.metadata
        ])
        
        node_types = json.stringify( {
        "formationFiles": {
          "AssembleFormationTask.h": "Manages the process of assembling units into specified formations",
          "CreateFormationTask.h": "Handles creation and initialization of new unit formations",
          "DeleteFormationTask.h": "Controls cleanup and removal of formations"
        },
        "movementFiles": {
          "SelectNextRouteSegmentTask.h": "Controls progression through waypoints",
          "NotifyRouteCompletedTask.h": "Handles route completion notifications",
          "IssueBoundingOverwatchTask.h": "Coordinates bounding overwatch movement",
          "IssueMovementOrdersTask.h": "Manages movement order distribution",
          "IssueUnitMoveTacOrderTask.h": "Handles tactical movement orders"
        },
        "combatFiles": {
          "IssueAttackOrdersTask.h": "Manages attack order distribution",
          "IssueDefendPositionOrdersTask.h": "Handles defensive position orders",
          "UnitAssignTargetsTask.h": "Controls target assignments",
          "UnitCanEngageTargetCondition.h": "Evaluates engagement capabilities",
          "UnitCombatPowerCondition.h": "Assesses relative combat strength"
        },
        "evaluatorFiles": {
          "UnitEnemySituationEvaluator.h": "Assesses enemy situations",
          "UnitHealthStateTreeEvaluator.h": "Monitors unit health status",
          "UnitHierarchyEvaluator.h": "Manages unit organization"
        },
        "tacticalFiles": {
          "IssueSubunitFindCoverTask.h": "Coordinates finding cover positions",
          "IssueSubunitOccupyCoverTask.h": "Manages occupying cover positions",
          "FindEngagementLocationTask.h": "Determines optimal engagement positions",
          "CancelSubOrdersTask.h": "Manages order cancellation"
        },
        "perceptionFiles": {
            "EntitySensedEntitiesComponent.h": "Manages individual entity sensory information",
            "EntitySightModeComponent.h": "Controls entity vision and detection capabilities",
            "EntityVisibilityComponent.h": "Handles entity visibility states and checks"
        },
        "movementFiles": {
            "EntityMovementComponent.h": "Controls individual entity movement",
            "EntityPathfindingComponent.h": "Manages pathfinding for individual entities",
            "EntityNavigationComponent.h": "Handles navigation and obstacle avoidance"
        },
        "combatFiles": {
            "EntityCombatComponent.h": "Manages individual combat capabilities",
            "EntityWeaponComponent.h": "Controls weapon systems and firing",
            "EntityTargetingComponent.h": "Handles target acquisition and tracking"
        },
        "healthFiles": {
            "EntityHealthComponent.h": "Tracks entity health and damage",
            "EntityStatusComponent.h": "Monitors entity status conditions",
            "EntityVitalityComponent.h": "Manages stamina and other vital stats"
        },
        "behaviorFiles": {
            "EntityBehaviorTreeComponent.h": "Controls individual AI decision making",
            "EntityStateComponent.h": "Manages entity state machines",
            "EntityTaskComponent.h": "Handles individual task execution"
        },
        "communicationFiles": {
            "EntityMessageComponent.h": "Manages entity communication",
            "EntitySignalComponent.h": "Handles signals and alerts",
            "EntityCommandComponent.h": "Processes received commands"
        }
      })

        # Create enhanced prompt
        enhanced_prompt = f"""[INST] 
        
        Use the following node types in the creation of the tree. Look at the node names and the corresponding description when determining what actions to make: 

        {node_types}
        
        Using the following context about military behavior trees:

        {context}

        Generate a behavior tree in XML format for the following scenario:
        {prompt}
        
        The output should be a valid XML behavior tree. 
        
        [/INST]"""
        
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
            "context_used": context[:500] + "..." if len(context) > 500 else context,  # Save truncated context
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
        logger.info(f"Loading scenarios from {scenarios_file}")
        with open(scenarios_file, 'r') as f:
            data = json.load(f)
            scenarios = data.get('scenarios', [])  # Expect scenarios under 'scenarios' key
            
        if not scenarios:
            logger.error("No scenarios found in input file")
            return []
            
        logger.info(f"Found {len(scenarios)} scenarios to process")
            
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
        for i, scenario in enumerate(scenarios, 1):
            name = scenario.get('name', f'scenario_{i}')
            prompt = scenario.get('prompt', '')
            
            if not prompt:
                logger.warning(f"Skipping scenario '{name}': no prompt provided")
                continue
                
            logger.info(f"Processing scenario {i}/{len(scenarios)}: {name}")
            try:
                xml_path = generate_behavior_tree(
                    model,
                    tokenizer,
                    prompt,
                    name,
                    output_dir
                )
                generated_files.append(xml_path)
                logger.info(f"Successfully processed scenario {name}")
                
            except Exception as e:
                logger.error(f"Error processing scenario '{name}': {str(e)}")
                continue
        
        logger.info(f"Completed processing {len(generated_files)} out of {len(scenarios)} scenarios")
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
    
    if not os.path.exists(scenarios_file):
        print(f"Error: Scenarios file '{scenarios_file}' not found")
        sys.exit(1)
        
    try:
        generated_files = process_scenarios(scenarios_file, output_dir)
        print(f"\nGenerated {len(generated_files)} behavior trees:")
        for file in generated_files:
            print(f"- {file}")
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)