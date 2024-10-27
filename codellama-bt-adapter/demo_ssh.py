import torch
from transformers import AutoModelForCausalLM, AutoTokenizer
from peft import PeftModel
import datetime
import os
import json
import logging
import paramiko
from pathlib import Path
from retrieve_context import retrieve_similar_content
from dotenv import load_dotenv

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class GPUConnection:
    def __init__(self, hostname, username, key_path=None, password=None):
        self.hostname = hostname
        self.username = username
        self.key_path = key_path
        self.password = password
        self.ssh_client = None
        self.sftp_client = None

    def connect(self):
        """Establish SSH connection to GPU server"""
        try:
            self.ssh_client = paramiko.SSHClient()
            self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

            connect_params = {
                'hostname': self.hostname,
                'username': self.username,
            }

            if self.key_path:
                connect_params['key_filename'] = os.path.expanduser(self.key_path)
            elif self.password:
                connect_params['password'] = self.password

            logger.info(f"Connecting to {self.hostname}...")
            self.ssh_client.connect(**connect_params)
            self.sftp_client = self.ssh_client.open_sftp()
            logger.info("Connected successfully")

        except Exception as e:
            logger.error(f"Failed to connect: {str(e)}")
            raise

    def disconnect(self):
        """Close SSH connection"""
        if self.sftp_client:
            self.sftp_client.close()
        if self.ssh_client:
            self.ssh_client.close()
        logger.info("Disconnected from GPU server")

    def execute_command(self, command, working_dir=None):
        """Execute command on remote server"""
        if working_dir:
            command = f"cd {working_dir} && {command}"
        
        logger.debug(f"Executing command: {command}")
        stdin, stdout, stderr = self.ssh_client.exec_command(command)
        
        output = stdout.read().decode()
        error = stderr.read().decode()
        
        if error:
            logger.warning(f"Command produced error output: {error}")
        
        return output, error

    def check_gpu_status(self, working_dir):
        """Check GPU status using bash script"""
        output, error = self.execute_command(
            f"./run_model.sh gpu-status",
            working_dir
        )
        if error:
            raise Exception(f"GPU status check failed: {error}")
        return output

    def initialize_model(self, working_dir):
        """Initialize model using bash script"""
        output, error = self.execute_command(
            f"./run_model.sh init",
            working_dir
        )
        if error:
            raise Exception(f"Model initialization failed: {error}")
        return output

    def generate_response(self, prompt, working_dir):
        """Generate response using bash script"""
        try:
            # Create temporary files
            prompt_file = f"/tmp/prompt_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
            response_file = f"/tmp/response_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.json"

            # Upload prompt
            with self.sftp_client.file(prompt_file, 'w') as f:
                f.write(prompt)

            # Generate response
            output, error = self.execute_command(
                f"./run_model.sh generate {prompt_file} {response_file}",
                working_dir
            )

            if error:
                raise Exception(f"Generation failed: {error}")

            # Read response
            with self.sftp_client.file(response_file, 'r') as f:
                response_data = json.load(f)

            if response_data['status'] != 'success':
                raise Exception(response_data.get('error', 'Unknown error'))

            return response_data['response']

        finally:
            # Cleanup temporary files
            try:
                self.execute_command(f"rm -f {prompt_file} {response_file}")
            except:
                pass

def run_demo():
    """Main demo function"""
    load_dotenv()

    # GPU connection configuration
    gpu_config = {
        'hostname': os.getenv('GPU_HOSTNAME'),
        'username': os.getenv('GPU_USERNAME'),
        'key_path': os.getenv('GPU_KEY_PATH')
    }

    # Remote paths configuration
    REMOTE_WORKING_DIR = os.getenv('REMOTE_WORKING_DIR', '~/bt_adapter')
    
    gpu_connection = None
    try:
        # Initialize GPU connection
        gpu_connection = GPUConnection(**gpu_config)
        gpu_connection.connect()

        # Check GPU status
        logger.info("Checking GPU status...")
        gpu_status = gpu_connection.check_gpu_status(REMOTE_WORKING_DIR)
        logger.info(f"GPU Status:\n{gpu_status}")

        # Initialize model
        logger.info("Initializing model...")
        init_output = gpu_connection.initialize_model(REMOTE_WORKING_DIR)
        logger.info(f"Model initialization output:\n{init_output}")

        while True:
            print("\nEnter your behavior tree query (or 'quit' to exit):")
            query = input().strip()

            if query.lower() == 'quit':
                break

            try:
                # Get context from Pinecone
                print("Retrieving relevant context...")
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
                print("Generating behavior tree...")
                response = gpu_connection.generate_response(
                    enhanced_prompt,
                    REMOTE_WORKING_DIR
                )

                print("\nGenerated Behavior Tree:")
                print(response)

            except Exception as e:
                logger.error(f"Error during generation: {str(e)}")
                print(f"\nAn error occurred: {str(e)}")
                continue

    except Exception as e:
        logger.error(f"An error occurred: {str(e)}")
        raise

    finally:
        if gpu_connection:
            gpu_connection.disconnect()

if __name__ == "__main__":
    run_demo()