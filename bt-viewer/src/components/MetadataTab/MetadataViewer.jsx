import React from 'react';
import Editor from "@monaco-editor/react";
import FormattedMetadata from './FormattedMetadata';
import { EyeIcon, CodeBracketIcon } from '@heroicons/react/24/outline';

const MetadataViewer = ({ metadata }) => {
  const [viewMode, setViewMode] = React.useState('formatted');
  const formattedMetadata = JSON.stringify(metadata, null, 2);

  return (
    <div className="space-y-4">
      <div className="flex justify-end space-x-2">
        <button
          onClick={() => setViewMode('formatted')}
          className={`inline-flex items-center px-3 py-1 rounded ${
            viewMode === 'formatted' ? 'bg-indigo-600 text-white' : 'bg-gray-200 text-gray-700'
          }`}
        >
          <EyeIcon className="w-4 h-4 mr-1" />
          Formatted
        </button>
        <button
          onClick={() => setViewMode('code')}
          className={`inline-flex items-center px-3 py-1 rounded ${
            viewMode === 'code' ? 'bg-indigo-600 text-white' : 'bg-gray-200 text-gray-700'
          }`}
        >
          <CodeBracketIcon className="w-4 h-4 mr-1" />
          JSON
        </button>
      </div>

      <div className="h-[600px] border rounded-lg overflow-hidden">
        {viewMode === 'formatted' ? (
          <div className="h-full overflow-y-auto p-6">
            <FormattedMetadata metadata={metadata} />
          </div>
        ) : (
          <Editor
            height="100%"
            defaultLanguage="json"
            value={formattedMetadata}
            options={{
              minimap: { enabled: false },
              readOnly: true,
              scrollBeyondLastLine: false,
              wordWrap: 'on',
              formatOnPaste: true,
              formatOnType: true,
            }}
            theme="vs-light"
          />
        )}
      </div>
    </div>
  );
};

export default MetadataViewer;