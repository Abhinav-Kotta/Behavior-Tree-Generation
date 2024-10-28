import React from 'react';
import XMLPreview from './XMLPreview';
import BehaviorTreeVisualizer from './BehaviorTreeVisualizer';

const BehaviorTreeViewer = ({ xml }) => {
  const [viewMode, setViewMode] = React.useState('visual');

  return (
    <div className="space-y-4">
      <div className="flex justify-end space-x-2">
        <button
          onClick={() => setViewMode('visual')}
          className={`px-3 py-1 rounded ${
            viewMode === 'visual' ? 'bg-blue-600 text-white' : 'bg-gray-200'
          }`}
        >
          Visual
        </button>
        <button
          onClick={() => setViewMode('code')}
          className={`px-3 py-1 rounded ${
            viewMode === 'code' ? 'bg-blue-600 text-white' : 'bg-gray-200'
          }`}
        >
          XML
        </button>
      </div>

      <div className="h-[600px] border rounded-lg overflow-hidden">
        {viewMode === 'visual' ? (
          <BehaviorTreeVisualizer xml={xml} />
        ) : (
          <XMLPreview xml={xml} />
        )}
      </div>
    </div>
  );
};

export default BehaviorTreeViewer;