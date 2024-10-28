import React from 'react';
import { Tab } from '@headlessui/react';
import BehaviorTreeViewer from '../BehaviorTreeTab/BehaviorTreeViewer';
import MetadataViewer from '../MetadataTab/MetadataViewer';

function classNames(...classes) {
  return classes.filter(Boolean).join(' ');
}

const TabLayout = ({ xml, metadata }) => {
  const tabs = [
    { name: 'Behavior Tree', content: <BehaviorTreeViewer xml={xml} /> },
    { name: 'Metadata', content: <MetadataViewer metadata={metadata} /> },
  ];

  return (
    <div className="w-full">
      <Tab.Group>
        <div className="border-b border-gray-200 mb-4">
          <Tab.List className="relative flex space-x-8">
            {tabs.map((tab, index) => (
              <Tab
                key={tab.name}
                className={({ selected }) =>
                  classNames(
                    'py-4 px-1 relative',
                    'focus:outline-none',
                    selected
                      ? 'text-indigo-600 font-medium'
                      : 'text-gray-500 hover:text-gray-700'
                  )
                }
              >
                {({ selected }) => (
                  <>
                    {tab.name}
                    {selected && (
                      <div className="absolute bottom-0 left-0 right-0 h-0.5 bg-indigo-600 transition-all duration-300" />
                    )}
                  </>
                )}
              </Tab>
            ))}
          </Tab.List>
        </div>
        <Tab.Panels className="mt-2">
          {tabs.map((tab, idx) => (
            <Tab.Panel
              key={idx}
              className={classNames(
                'rounded-xl bg-white p-4',
                'ring-white ring-opacity-60 ring-offset-2 ring-offset-blue-400'
              )}
            >
              {tab.content}
            </Tab.Panel>
          ))}
        </Tab.Panels>
      </Tab.Group>
    </div>
  );
}

export default TabLayout;