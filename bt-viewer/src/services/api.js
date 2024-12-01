/**
 * Executive Summary:
 * This file contains the API service for the Behavior Tree Viewer application. It provides methods for generating behavior trees and downloading the generated XML and metadata files. 
 * The generateBehaviorTree function submits a job to the backend server to generate a behavior tree based on a given scenario name and prompt. The downloadXML and downloadMetadata functions are used to download the generated XML and metadata files, respectively.
 */

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000';

/**
 * Generates a behavior tree by submitting a job through submit_job.sh
 * This can be switched out with a different service to generate the behavior tree as long as the inputs and outputs are modified. 
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

    // Check response came back
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

/**
 * Downloads the XML file.
 * @param {string} xml - The XML content to be downloaded
 * @param {string} filename - The name of the file to be downloaded
 */
/**
 * Downloads the XML file.
 * @param {string} xml - The XML content to be downloaded
 * @param {string} filename - The name of the file to be downloaded
 */
export const downloadXML = (xml, filename) => {
  // Create a blob from the XML content with the correct MIME type
  const blob = new Blob([xml], { type: 'application/xml' });
  // Generate a URL for the blob
  const url = window.URL.createObjectURL(blob);
  // Create a temporary anchor element to simulate a click
  const a = document.createElement('a');
  // Set the href attribute to the blob URL
  a.href = url;
  // Set the download attribute to the filename with an XML extension
  a.download = `${filename}.xml`;
  // Add the anchor element to the document body
  document.body.appendChild(a);
  // Simulate a click on the anchor element to download the file
  a.click();
  // Revoke the blob URL to free up memory
  window.URL.revokeObjectURL(url);
  // Remove the anchor element from the document body
  document.body.removeChild(a);
};

/**
 * Downloads the metadata file.
 * @param {Object} metadata - The metadata to be downloaded
 * @param {string} filename - The name of the file to be downloaded
 */
/**
 * Downloads the metadata file.
 * @param {Object} metadata - The metadata to be downloaded
 * @param {string} filename - The name of the file to be downloaded
 */
export const downloadMetadata = (metadata, filename) => {
  // Convert the metadata object to a JSON string with indentation for readability
  const metadataString = JSON.stringify(metadata, null, 2);
  // Create a blob from the metadata string with the correct MIME type
  const blob = new Blob([metadataString], { type: 'application/json' });
  // Generate a URL for the blob
  const url = window.URL.createObjectURL(blob);
  // Create a temporary anchor element to simulate a click
  const a = document.createElement('a');
  // Set the href attribute to the blob URL
  a.href = url;
  // Set the download attribute to the filename with a JSON extension
  a.download = `${filename}.json`;
  // Add the anchor element to the document body
  document.body.appendChild(a);
  // Simulate a click on the anchor element to download the file
  a.click();
  // Revoke the blob URL to free up memory
  window.URL.revokeObjectURL(url);
  // Remove the anchor element from the document body
  document.body.removeChild(a);
};

/**
 * Exports the functions for generating behavior tree, downloading XML and downloading metadata.
 */
export default {
  generateBehaviorTree,
  downloadXML,
  downloadMetadata
};