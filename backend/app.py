from flask import Flask, request, jsonify
from flask_cors import CORS
import sys
import os
from dotenv import load_dotenv

# Add bt_adapter directory to Python path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'bt_adapter'))

from demo import initialize_model, generate_and_save_response
from retrieve_context import retrieve_similar_content

load_dotenv()

app = Flask(__name__)
CORS(app)

# Initialize model globally
BASE_MODEL_PATH = os.getenv('BASE_MODEL_PATH')
LORA_ADAPTER_PATH = os.getenv('LORA_ADAPTER_PATH')
model, tokenizer = initialize_model(BASE_MODEL_PATH, LORA_ADAPTER_PATH)

@app.route('/generate', methods=['POST'])
def generate_tree():
    data = request.json
    query = data.get('prompt')

    situationInfo = {
  "Information": {
    "Specific node subcategories pertaining to units": {
      "formationFiles": {
        "AssembleFormationTask.h": "Manages the process of assembling units into specified formations and positions, including validation and execution of formation orders",
        "CreateFormationTask.h": "Handles creation and initialization of new unit formations, including formation selection and hierarchy setup",
        "DeleteFormationTask.h": "Controls the cleanup and removal of formations, including proper dismantling of formation structures"
      },
      "movementFiles": {
        "SelectNextRouteSegmentTask.h": "Controls progression through waypoints along a defined route, managing route segments",
        "NotifyRouteCompletedTask.h": "Handles notifications when units complete their assigned routes, including parent unit notifications",
        "IssueBoundingOverwatchTask.h": "Coordinates alternating movement between two units for tactical advancement using bounding overwatch technique",
        "IssueMovementOrdersTask.h": "Manages creation and distribution of basic movement orders to units and subunits",
        "IssueUnitMoveTacOrderTask.h": "Specializes in tactical movement orders for specific units, including formation maintenance"
      },
      "combatFiles": {
        "IssueAttackOrdersTask.h": "Handles creation and distribution of attack orders, including coordination between units",
        "IssueDefendPositionOrdersTask.h": "Manages defensive position orders and coordination of unit defensive arrangements",
        "UnitAssignTargetsTask.h": "Controls the distribution of target assignments among unit members based on tactical priorities",
        "UnitCanEngageTargetCondition.h": "Evaluates whether units can effectively engage specific targets based on range and capabilities",
        "UnitCombatPowerCondition.h": "Assesses relative combat strength between friendly and enemy forces"
      },
      "evaluatorFiles": {
        "UnitEnemySituationEvaluator.h": "Continuously assesses enemy presence, threat levels, and tactical situation",
        "UnitHealthStateTreeEvaluator.h": "Monitors unit health, combat effectiveness, and overall unit status",
        "UnitHierarchyEvaluator.h": "Manages and evaluates unit organizational structure and command relationships"
      },
      "eventFiles": {
        "SignalUnitEnemySpottedEvent.h": "Manages the propagation of enemy detection notifications through the unit hierarchy",
        "UnitChangedEvent.h": "Handles events related to changes in unit composition or structure",
        "UnitMemberDestroyedEvent.h": "Processes events related to unit member casualties and their impact on the unit"
      },
      "tacticalFiles": {
        "IssueSubunitFindCoverTask.h": "Coordinates subunits in searching for and evaluating potential cover positions",
        "IssueSubunitOccupyCoverTask.h": "Manages the process of units moving to and occupying identified cover positions",
        "FindEngagementLocationTask.h": "Determines optimal positions for unit engagement based on tactical considerations",
        "CancelSubOrdersTask.h": "Manages the cancellation and cleanup of existing orders for subunits"
      },
      "conditionFiles": {
        "CheckTrueLeaderKilledCondition.h": "Evaluates and responds to the loss of unit leadership",
        "CheckUnitPlatformCondition.h": "Verifies unit type, capabilities, and platform-specific requirements",
        "MovementTechniqueCompareCondition.h": "Compares and evaluates different movement techniques for tactical situations"
      },
      "coordinationFiles": {
        "WaitForAllUnitOrderCompletionTask.h": "Synchronizes completion of multiple unit orders before proceeding",
        "WaitForBoundingOverwatchCompletion.h": "Coordinates the completion of bounding overwatch movement between units",
        "RemoveDeadEntitiesTask.h": "Manages the removal and accounting of casualties from unit formations",
        "IssueHaltFRAGOTask.h": "Handles immediate order changes and emergency stops for units"
      }
    }
  }
}
    
    try:
        # Get context from Pinecone
        results = retrieve_similar_content(query, "pdf-rag-index", top_k=3)
        
        # Extract context
        context = "\n\n".join([
            match.metadata['text'] 
            for match in results.matches 
            if hasattr(match, 'metadata') and 'text' in match.metadata
        ])
        
        # Create enhanced prompt
        enhanced_prompt = f"""[INST] Using the following context about military behavior trees:

        {context}

        Generate a behavior tree in XML format for the following scenario:
        {query} [/INST]
        
        Use the following node information in your decision
        {situationInfo}
        """
        
        # Generate response
        response = generate_and_save_response(
            model,
            tokenizer,
            enhanced_prompt,
            os.path.join(LORA_ADAPTER_PATH, "outputs/api_generation.txt"),
            max_new_tokens=512,
            temperature=0.7
        )
        
        return jsonify({
            'success': True,
            'response': response,
            'context_used': bool(context)
        })
        
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

if __name__ == '__main__':
    app.run(debug=True)
