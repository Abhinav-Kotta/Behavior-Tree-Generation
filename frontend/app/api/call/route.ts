import { NextApiRequest, NextApiResponse } from "next";
import { exec } from "child_process";
import { promisify } from "util";

const execPromise = promisify(exec);

export default async function handler(req: NextApiRequest, res: NextApiResponse) {
  if (req.method === "POST") {
    try {
      // Execute the shell script with the scenarios.json file as an argument
      const { stdout, stderr } = await execPromise("bash submit_job.sh scenarios.json");

      // Check for any errors
      if (stderr) {
        console.error("Error:", stderr);
        return res.status(500).json({ message: "Error executing script", error: stderr });
      }

      // Return the output from the script
      console.log("Output:", stdout);
      return res.status(200).json({ message: "Script executed successfully", output: stdout });
    } catch (error) {
      console.error("Execution failed:", error);
      return res.status(500).json({ message: "Script execution failed", error: error.message });
    }
  } else {
    // Handle other HTTP methods
    res.setHeader("Allow", ["POST"]);
    return res.status(405).end(`Method ${req.method} Not Allowed`);
  }
}
