import React from 'react';
import Editor from "@monaco-editor/react";

const XMLPreview = ({ xml }) => {
  return (
    <Editor
      height="100%"
      defaultLanguage="xml"
      value={xml}
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
  );
};

export default XMLPreview;