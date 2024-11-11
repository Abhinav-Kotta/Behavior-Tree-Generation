# Setting up project
1. Create Virtual environment \
``` conda create -n military_bt python=3.10 ```
2. ``` cd ``` into the backend directory and run \
``` pip install -r requirements.txt ```
3. Pull the codellama model from huggingface into your local directory using the hugging face CLI \
```huggingface-cli download codellama/CodeLlama-7b-Instruct-hf```
- If you don't have the Huggine face CLI installed, click [here](https://huggingface.co/docs/huggingface_hub/main/en/guides/cli) to install and learn more
5. Run the shell script that submits the job to Newton \
``` ./start_dev.sh ```
