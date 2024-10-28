from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import List
import os
import json
import subprocess
from pathlib import Path

app = FastAPI()

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class Scenario(BaseModel):
    name: str
    prompt: str

class ScenarioRequest(BaseModel):
    scenarios: List[Scenario]

@app.post("/generate")
async def generate_behavior_tree(request: ScenarioRequest):
    try:
        print("\n=== Starting Behavior Tree Generation ===")
        print(f"Received scenario: {request.scenarios[0].name}")
        
        # Get root directory and scenarios.json path
        base_dir = Path(__file__).parent.parent.parent
        scenarios_file = base_dir / "scenarios.json"
        
        # Write to scenarios.json in root directory
        print(f"\nWriting to scenarios file: {scenarios_file}")
        with open(scenarios_file, 'w') as f:
            json.dump({"scenarios": [s.dict() for s in request.scenarios]}, f, indent=2)
        
        # Get submit script path
        submit_script = base_dir / "submit_job.sh"
        print(f"\nExecuting submit_job.sh:")
        print(f"Script path: {submit_script}")
        print(f"Working directory: {base_dir}")
        print(f"Using scenarios file: {scenarios_file}")

        # Run the submit_job.sh script with scenarios.json
        print("\nSubmitting job...")
        process = subprocess.run(
            [str(submit_script), str(scenarios_file)],
            capture_output=True,
            text=True,
            cwd=str(base_dir)
        )
        
        print("\n=== Job Submission Results ===")
        print(f"Return code: {process.returncode}")
        print(f"\nOutput:")
        print(process.stdout)
        
        if process.stderr:
            print(f"\nErrors:")
            print(process.stderr)

        if process.returncode != 0:
            raise HTTPException(
                status_code=500, 
                detail=f"Job submission failed: {process.stderr}"
            )

        # Look for output files
        output_dir = base_dir / "behavior_trees"
        print(f"\nLooking for output files in: {output_dir}")
        
        latest_files = {
            "xml": None,
            "metadata": None
        }

        if output_dir.exists():
            xml_files = list(output_dir.glob("xml/*.xml"))
            metadata_files = list(output_dir.glob("metadata/*.json"))

            if xml_files:
                latest_xml = max(xml_files, key=os.path.getctime)
                print(f"Found XML file: {latest_xml}")
                with open(latest_xml) as f:
                    latest_files["xml"] = f.read()

            if metadata_files:
                latest_metadata = max(metadata_files, key=os.path.getctime)
                print(f"Found metadata file: {latest_metadata}")
                with open(latest_metadata) as f:
                    latest_files["metadata"] = json.load(f)

        print("\n=== Generation Complete ===\n")
        return latest_files

    except Exception as e:
        print(f"\nERROR: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    import uvicorn
    print("Starting FastAPI server...")
    uvicorn.run(app, host="0.0.0.0", port=8000)