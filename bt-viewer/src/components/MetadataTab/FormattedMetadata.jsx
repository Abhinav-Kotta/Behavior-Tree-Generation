import React from 'react';

const MetadataField = ({ label, value }) => (
  <div className="py-3 border-b border-gray-200 last:border-b-0">
    <dt className="text-sm font-medium text-gray-500">{label}</dt>
    <dd className="mt-1 text-sm text-gray-900">
      {typeof value === 'object' 
        ? <pre className="mt-2 text-sm text-gray-700 whitespace-pre-wrap">{JSON.stringify(value, null, 2)}</pre>
        : value
      }
    </dd>
  </div>
);

const FormattedMetadata = ({ metadata }) => {
  const formatDate = (timestamp) => {
    const date = new Date(timestamp);
    return date.toLocaleString();
  };

  return (
    <div className="bg-white rounded-lg">
      <dl className="divide-y divide-gray-200">
        <MetadataField 
          label="Timestamp" 
          value={metadata.timestamp} 
        />
        <MetadataField 
          label="Scenario Name" 
          value={metadata.scenario} 
        />
        <MetadataField 
          label="Prompt" 
          value={metadata.prompt} 
        />
        <MetadataField 
          label="Context Chunks Used" 
          value={metadata.context_chunks} 
        />
        <div className="py-3">
          <dt className="text-sm font-medium text-gray-500">Context Sample</dt>
          <dd className="mt-1">
            <div className="bg-gray-50 rounded-md p-3 text-sm text-gray-700">
              {metadata.context_used}
            </div>
          </dd>
        </div>
        <div className="py-3">
          <dt className="text-sm font-medium text-gray-500">Generation Parameters</dt>
          <dd className="mt-1">
            <dl className="grid grid-cols-3 gap-4 text-sm">
              {Object.entries(metadata.generation_params).map(([key, value]) => (
                <div key={key}>
                  <dt className="font-medium text-gray-500">{key}</dt>
                  <dd className="mt-1 text-gray-900">{value}</dd>
                </div>
              ))}
            </dl>
          </dd>
        </div>
      </dl>
    </div>
  );
};

export default FormattedMetadata;
