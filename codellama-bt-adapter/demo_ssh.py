DEFAULT_MESSAGES = [
    {
        "role": "system", 
        "content": "You are an AI assistant specialized in creating military behavior trees."
    },
    {
        "role": "user",
        "content": """Use the following node types in the creation of the tree:
{node_types}

Context about military behavior trees:
{context}

Generate a behavior tree in XML format for:
{prompt}

The output should be a valid XML behavior tree."""
    }
]

def format_chat_prompt(messages):
    system_msg = messages[0]['content']
    user_msg = messages[1]['content']
    return f"<s>[INST] <<SYS>>\n{system_msg}\n<</SYS>>\n\n{user_msg} [/INST]"

# demo_ssh.py
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

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def initialize_model(base_model_path, lora_adapter_path):
    try:
        logger.info(f"CUDA available: {torch.cuda.is_available()}")
        if torch.cuda.is_available():
            logger.info(f"GPU Device: {torch.cuda.get_device_name(0)}")
        
        tokenizer = AutoTokenizer.from_pretrained(base_model_path, trust_remote_code=True)
        tokenizer.pad_token = tokenizer.eos_token
        
        model = AutoModelForCausalLM.from_pretrained(
            base_model_path,
            torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
            device_map="auto" if torch.cuda.is_available() else None,
            trust_remote_code=True
        )
        
        model = PeftModel.from_pretrained(
            model,
            lora_adapter_path,
            torch_dtype=torch.float16 if torch.cuda.is_available() else torch.float32,
        )
        
        if torch.cuda.is_available():
            model.half()
        
        model.eval()
        return model, tokenizer

    except Exception as e:
        logger.error(f"Error initializing model: {str(e)}")
        raise

def extract_xml_from_response(response):
    try:
        start_idx = response.find('<?xml')
        if start_idx == -1:
            start_idx = response.find('<behavior')
        if start_idx == -1:
            start_idx = response.find('<root')
            
        if start_idx == -1:
            logger.warning("No XML tags found in response")
            return response
            
        end_idx = response.rfind('>')
        if end_idx == -1:
            logger.warning("No closing tag found in response")
            return response
            
        xml_content = response[start_idx:end_idx+1]
        
        try:
            dom = xml.dom.minidom.parseString(xml_content)
            pretty_xml = dom.toprettyxml(indent="  ")
            return '\n'.join([line for line in pretty_xml.split('\n') if line.strip()])
        except Exception as e:
            logger.warning(f"Failed to format XML: {str(e)}")
            return xml_content
            
    except Exception as e:
        logger.error(f"Error extracting XML: {str(e)}")
        return response

def generate_behavior_tree(model, tokenizer, prompt, scenario_name, output_dir, max_new_tokens=512, temperature=0.7, top_p=0.95):
    try:
        logger.info(f"Generating behavior tree for scenario: {scenario_name}")
        
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

        formatted_messages = [
            {**msg, 'content': msg['content'].format(
                node_types=json.dumps(node_types),
                context=context,
                prompt=prompt
            ) if msg['role'] == 'user' else msg['content']}
            for msg in DEFAULT_MESSAGES
        ]

        formatted_prompt = format_chat_prompt(formatted_messages)        
        inputs = tokenizer(formatted_prompt, return_tensors="pt").to(model.device)
        
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
        xml_content = extract_xml_from_response(response)
        
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        xml_filename = f"{scenario_name}_{timestamp}.xml"
        xml_path = os.path.join(output_dir, "xml", xml_filename)
        os.makedirs(os.path.dirname(xml_path), exist_ok=True)
        
        with open(xml_path, 'w', encoding='utf-8') as f:
            f.write(xml_content)
        
        metadata = {
            "timestamp": timestamp,
            "scenario": scenario_name,
            "prompt": prompt,
            "context_chunks": len(results.matches),
            "context_used": context[:500] + "..." if len(context) > 500 else context,
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
            
        return xml_path

    except Exception as e:
        logger.error(f"Error generating behavior tree: {str(e)}")
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