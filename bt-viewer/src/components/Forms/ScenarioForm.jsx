import React from 'react';

const ScenarioForm = ({ onSubmit, disabled }) => {
  const [formData, setFormData] = React.useState({
    name: '',
    prompt: ''
  });

  const handleSubmit = (e) => {
    e.preventDefault();
    onSubmit(formData);
  };

  const handleChange = (e) => {
    setFormData(prev => ({
      ...prev,
      [e.target.name]: e.target.value
    }));
  };

  return (
    <form onSubmit={handleSubmit} className="w-full space-y-6">
      <div className="grid grid-cols-1 gap-6">
        <div>
          <label htmlFor="name" className="block text-lg font-medium text-gray-700 mb-2">
            Scenario Name
          </label>
          <input
            type="text"
            name="name"
            id="name"
            value={formData.name}
            onChange={handleChange}
            disabled={disabled}
            required
            className="w-full px-4 py-3 text-gray-900 border border-gray-300 rounded-lg focus:ring-2 focus:ring-indigo-500 focus:border-indigo-500"
            placeholder="Enter a name for your scenario (e.g., patrol_mission)"
          />
        </div>

        <div>
          <label htmlFor="prompt" className="block text-lg font-medium text-gray-700 mb-2">
            Scenario Prompt
          </label>
          <textarea
            name="prompt"
            id="prompt"
            value={formData.prompt}
            onChange={handleChange}
            disabled={disabled}
            required
            rows={6}
            className="w-full px-4 py-3 text-gray-900 border border-gray-300 rounded-lg focus:ring-2 focus:ring-indigo-500 focus:border-indigo-500"
            placeholder="Describe the military scenario for the behavior tree (e.g., Create a behavior tree for an infantry patrol unit...)"
          />
        </div>

        <div>
          <button
            type="submit"
            disabled={disabled}
            className={`w-full flex justify-center py-4 px-6 text-lg font-medium rounded-lg text-white
              ${disabled ? 'bg-indigo-400 cursor-not-allowed' : 'bg-indigo-600 hover:bg-indigo-700'}
              focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500
              transition-colors duration-200`}
          >
            {disabled ? 'Generating...' : 'Generate Behavior Tree'}
          </button>
        </div>
      </div>
    </form>
  );
};

export default ScenarioForm;