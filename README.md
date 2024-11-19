# Setting up the Project

Follow these steps to set up the project:
1. Clone the repo into both your local and Newton account as there will be transferring of between local and Newton accounts.
2. Create a virtual environment using conda:
   ```bash
   conda create -n military_bt python=3.10
   ```
3. Navigate to the backend directory and install the required packages:
    ```bash
    cd backend
    pip install -r requirements.txt
    ```
4. Pull the CodeLLaMa model from Hugging Face into your local directory using the Hugging Face CLI:
    ```bash
    huggingface-cli download codellama/CodeLlama-7b-Instruct-hf
    ```
    - If you don't have the Hugging Face CLI installed, follow the instructions [here](https://huggingface.co/docs/huggingface_hub/main/en/guides/cli) to install and learn more.
    - Navigate to your cache located at `/home/username/.cache/huggingface/hub` and locate the CodeLLaMa model.
    - Copy the model into the `models/codellama` directory.
5. Set up your `.env` file with the following variables:
    ```bash
    PINECONE_API_KEY=

    REMOTE_HOST=newton.ist.ucf.edu
    REMOTE_USER=
    SSH_KEY_PATH=
    REMOTE_DIR= # create a directory in Newton for the files to copy over

    BASE_MODEL_PATH= # This should contain the path to the directory within the snapshots folder
    LORA_ADAPTER_PATH=codellama-bt-adapter
    ```
6. Run the shell script to submit the job to Newton:
    ```bash
    ./start_dev.sh
    ```