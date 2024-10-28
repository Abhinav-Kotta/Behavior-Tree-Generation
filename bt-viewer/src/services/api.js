const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000';

/**
 * Generates a behavior tree by submitting a job through submit_job.sh
 * @param {string} name - The name of the scenario
 * @param {string} prompt - The scenario prompt
 * @returns {Promise<{xml: string, metadata: Object}>} The generated behavior tree and metadata
 * @throws {Error} If the job submission or file generation fails
 */
export const generateBehaviorTree = async (name, prompt) => {
  try {
    const response = await fetch(`${API_URL}/generate`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        scenarios: [{
          name,
          prompt
        }]
      })
    });

    if (!response.ok) {
      const errorData = await response.json();
      throw new Error(errorData.detail || 'Failed to generate behavior tree');
    }

    const data = await response.json();
    
    // Validate response data
    if (!data.xml || !data.metadata) {
      throw new Error('Failed to generate behavior tree files');
    }

    return {
      xml: data.xml,
      metadata: data.metadata
    };
  } catch (error) {
    console.error('Generation Error:', error);
    throw new Error(
      error.message || 'Failed to generate behavior tree. Please check the server logs.'
    );
  }
};

export const downloadXML = (xml, filename) => {
  const blob = new Blob([xml], { type: 'application/xml' });
  const url = window.URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `${filename}.xml`;
  document.body.appendChild(a);
  a.click();
  window.URL.revokeObjectURL(url);
  document.body.removeChild(a);
};

export const downloadMetadata = (metadata, filename) => {
  const blob = new Blob([JSON.stringify(metadata, null, 2)], { type: 'application/json' });
  const url = window.URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `${filename}.json`;
  document.body.appendChild(a);
  a.click();
  window.URL.revokeObjectURL(url);
  document.body.removeChild(a);
};

export default {
  generateBehaviorTree,
  downloadXML,
  downloadMetadata
};