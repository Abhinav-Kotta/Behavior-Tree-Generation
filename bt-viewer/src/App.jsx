import React from 'react';
import TabLayout from './components/Tabs/TabLayout';
import ScenarioForm from './components/Forms/ScenarioForm';
import LoadingMessages from './components/LoadingMessages';
import { generateBehaviorTree, downloadXML, downloadMetadata } from './services/api';
import { ArrowDownTrayIcon } from '@heroicons/react/24/outline';

function App() {
  const [data, setData] = React.useState({
    xml: null,
    metadata: null,
  });
  const [loading, setLoading] = React.useState(false);
  const [error, setError] = React.useState(null);
  const [scenarioName, setScenarioName] = React.useState('');

  const handleSubmit = async (formData) => {
    try {
      setLoading(true);
      setError(null);
      setScenarioName(formData.name);
      const result = await generateBehaviorTree(formData.name, formData.prompt);
      setData(result);
    } catch (error) {
      setError(error.message);
      console.error('Error:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleDownloadXML = () => {
    if (data.xml) {
      downloadXML(data.xml, `${scenarioName}_behavior_tree`);
    }
  };

  const handleDownloadMetadata = () => {
    if (data.metadata) {
      downloadMetadata(data.metadata, `${scenarioName}_metadata`);
    }
  };

  const DownloadButton = ({ onClick, text }) => (
    <button
      onClick={onClick}
      className="inline-flex items-center px-4 py-2 bg-indigo-600 hover:bg-indigo-700 text-white 
                 rounded-md text-sm font-medium transition-colors duration-150 ease-in-out"
    >
      <ArrowDownTrayIcon className="w-4 h-4 mr-2" />
      {text}
    </button>
  );

  return (
    <div className="min-h-screen w-screen bg-gray-100 p-8">
      <div className="w-full max-w-7xl mx-auto">
        <div className="bg-white rounded-xl shadow-lg p-8 md:p-12">
          <div className="w-full">
            <div className="space-y-8">
              <div>
                <h1 className="text-4xl font-bold text-gray-900 text-center">
                  Behavior Tree Generator
                </h1>
                <p className="mt-2 text-lg text-gray-600 text-center">
                  Generate military behavior trees with context-aware AI
                </p>
              </div>
              
              <div className="w-full">
                <ScenarioForm onSubmit={handleSubmit} disabled={loading} />
                {error && (
                  <div className="mt-4 p-4 bg-red-100 text-red-700 rounded-md">
                    {error}
                  </div>
                )}
                {loading && <LoadingMessages />}
              </div>
              
              {(data.xml || data.metadata) && (
                <div className="w-full border-t border-gray-200 pt-8 space-y-4">
                  {/* Download buttons */}
                  <div className="flex justify-end space-x-4 mb-4">
                    <DownloadButton 
                      onClick={handleDownloadXML} 
                      text="Download XML"
                    />
                    <DownloadButton 
                      onClick={handleDownloadMetadata} 
                      text="Download Metadata"
                    />
                  </div>

                  {/* Tabs */}
                  <TabLayout 
                    xml={data.xml} 
                    metadata={data.metadata} 
                  />
                </div>
              )}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;