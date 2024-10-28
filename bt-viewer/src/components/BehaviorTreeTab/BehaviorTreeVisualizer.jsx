import React from 'react';
import ReactFlow, { 
  Controls, 
  Background,
  MarkerType,
  useNodesState,
  useEdgesState
} from 'react-flow-renderer';

const BehaviorTreeVisualizer = ({ xml }) => {
  const [nodes, setNodes, onNodesChange] = useNodesState([]);
  const [edges, setEdges, onEdgesChange] = useEdgesState([]);

  // Define node types and their colors
  const nodeTypes = {
    sequence: { color: '#4B5563', label: 'Sequence' },     // gray-600
    selector: { color: '#1D4ED8', label: 'Selector' },     // blue-700
    condition: { color: '#047857', label: 'Condition' },   // emerald-700
    action: { color: '#B91C1C', label: 'Action' }          // red-700
  };

  // Function to get color based on node type
  const getNodeColor = (nodeType) => {
    const type = nodeType.toLowerCase();
    if (type.includes('sequence')) return nodeTypes.sequence.color;
    if (type.includes('fallback') || type.includes('selector')) return nodeTypes.selector.color;
    if (type.includes('condition')) return nodeTypes.condition.color;
    if (type.includes('action')) return nodeTypes.action.color;
    return '#6B7280'; // gray-500 default
  };

  // Legend component
  const Legend = () => (
    <div className="absolute bottom-8 right-8 bg-white p-4 rounded-lg shadow-lg border border-gray-200 z-50">
      <h3 className="text-sm font-semibold mb-2 text-gray-700">Node Types</h3>
      <div className="space-y-2">
        {Object.entries(nodeTypes).map(([type, { color, label }]) => (
          <div key={type} className="flex items-center space-x-2">
            <div 
              className="w-4 h-4 rounded"
              style={{ backgroundColor: color }}
            />
            <span className="text-sm text-gray-600">{label}</span>
          </div>
        ))}
      </div>
      <div className="mt-4 pt-2 border-t border-gray-200">
        <p className="text-xs text-gray-500">Tip: Drag nodes to rearrange</p>
        <p className="text-xs text-gray-500">Use mouse wheel to zoom</p>
      </div>
    </div>
  );

  React.useEffect(() => {
    if (!xml) return;

    try {
      const parser = new DOMParser();
      const xmlDoc = parser.parseFromString(xml, 'text/xml');
      const mainTree = xmlDoc.querySelector('BehaviorTree');
      
      if (!mainTree) return;

      const processedNodes = [];
      const processedEdges = [];
      let nodeId = 0;
      
      const processNode = (element, parentId = null, x = 0, y = 0) => {
        const currentId = `node_${nodeId++}`;
        const nodeType = element.tagName;
        const nodeName = element.getAttribute('name') || element.getAttribute('ID') || nodeType;
        
        processedNodes.push({
          id: currentId,
          type: 'default',
          data: { 
            label: nodeName,
          },
          position: { x, y },
          draggable: true, // Enable dragging
          style: {
            background: getNodeColor(nodeType),
            color: 'white',
            border: '1px solid #555',
            borderRadius: '3px',
            padding: '10px',
            fontSize: '12px',
            fontWeight: '500',
            width: 'auto',
            minWidth: '100px',
            textAlign: 'center',
            cursor: 'move', // Show move cursor
            boxShadow: '0 2px 4px rgba(0,0,0,0.1)',
            transition: 'all 0.2s ease', // Smooth transitions
            '&:hover': {
              boxShadow: '0 4px 8px rgba(0,0,0,0.2)',
              transform: 'translateY(-1px)'
            }
          },
        });

        if (parentId) {
          processedEdges.push({
            id: `edge_${parentId}_${currentId}`,
            source: parentId,
            target: currentId,
            type: 'smoothstep',
            markerEnd: {
              type: MarkerType.ArrowClosed,
            },
            style: {
              stroke: '#64748B', // slate-500
              strokeWidth: 2,
              transition: 'stroke-width 0.2s ease', // Smooth edge transitions
            },
            animated: true, // Add edge animation
          });
        }

        const childNodes = Array.from(element.children);
        const childWidth = 250; // Increased spacing
        const startX = x - (childNodes.length - 1) * childWidth / 2;
        
        childNodes.forEach((child, index) => {
          processNode(
            child,
            currentId,
            startX + index * childWidth,
            y + 120 // Increased vertical spacing
          );
        });

        return currentId;
      };

      processNode(mainTree.children[0], null, 350, 50);
      
      setNodes(processedNodes);
      setEdges(processedEdges);
    } catch (error) {
      console.error('Error processing XML:', error);
    }
  }, [xml, setNodes, setEdges]);

  return (
    <div style={{ height: '600px' }}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        fitView
        minZoom={0.1}
        maxZoom={1.5}
        defaultZoom={0.8}
        snapToGrid={true}
        snapGrid={[15, 15]}
        nodesDraggable={true}
        elementsSelectable={true}
        zoomOnScroll={true}
        panOnScroll={true}
        fitViewOptions={{
          padding: 0.2,
          includeHiddenNodes: false,
        }}
      >
        <Controls 
          position="bottom-left"
          showInteractive={false}
        />
        <Background 
          variant="dots" 
          gap={12} 
          size={1} 
          color="#9CA3AF"
        />
        <Legend />
      </ReactFlow>
    </div>
  );
};

export default BehaviorTreeVisualizer;