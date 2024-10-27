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
        {query} [/INST]"""
        
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
