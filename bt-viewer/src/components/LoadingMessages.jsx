import React from 'react';

const LoadingMessages = () => {
  const messages = [
    "Retrieving military doctrine context...",
    "Analyzing scenario requirements...",
    "Generating behavior tree structure...",
    "Validating XML output...",
    "Applying military context to nodes...",
    "Processing RAG results...",
    "Formatting tree visualization...",
    "Almost there...",
  ];

  const [currentMessage, setCurrentMessage] = React.useState(0);

  React.useEffect(() => {
    const interval = setInterval(() => {
      setCurrentMessage((prev) => (prev + 1) % messages.length);
    }, 3000); // Change message every 3 seconds

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="mt-4 text-center">
      <div className="flex flex-col items-center space-y-4">
        <div className="animate-spin rounded-full h-12 w-12 border-4 border-indigo-600 border-t-transparent"></div>
        <div className="min-h-[2rem] text-gray-600">
          <p className="animate-fade-in">{messages[currentMessage]}</p>
        </div>
        <div className="text-sm text-gray-500">
          This may take a few minutes...
        </div>
      </div>
    </div>
  );
};

export default LoadingMessages;