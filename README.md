# Setting up the Project

Follow these steps to set up the project:

1. Create a virtual environment using conda:
   ```bash
   conda create -n military_bt python=3.10
   ```
2. Navigate to the backend directory and install the required packages:
    ```bash
    cd backend
    pip install -r requirements.txt
    ```
3. Pull the CodeLLaMa model from Hugging Face into your local directory using the Hugging Face CLI:
    ```bash
    huggingface-cli download codellama/CodeLlama-7b-Instruct-hf
    ```
    - If you don't have the Hugging Face CLI installed, follow the instructions [here](https://huggingface.co/docs/huggingface_hub/main/en/guides/cli) to install and learn more.
    - Navigate to your cache located at `/home/username/.cache/huggingface/hub` and locate the CodeLLaMa model.
    - Copy the model into the `models/codellama` directory.
4. Set up your `.env` file with the following variables:
    ```bash
    PINECONE_API_KEY=

    REMOTE_HOST=newton.ist.ucf.edu
    REMOTE_USER=
    SSH_KEY_PATH=
    REMOTE_DIR=

    BASE_MODEL_PATH=# This should contain the path to the directory within the snapshots folder
    LORA_ADAPTER_PATH=codellama-bt-adapter
    ```
5. Run the shell script to submit the job to Newton:
    ```bash
    ./start_dev.sh
    ```