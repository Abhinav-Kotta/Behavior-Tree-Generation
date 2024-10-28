"use client";

import { useState } from "react";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { ScrollArea } from "@/components/ui/scroll-area";

export default function BehaviorTree() {
  const [activeTab, setActiveTab] = useState("doctrine");

  const doctrineText = `
    Behavior Tree Doctrine:

    1. Root Node: The starting point of the behavior tree.
    2. Sequence Node: Executes child nodes in order until one fails.
    3. Selector Node: Executes child nodes in order until one succeeds.
    4. Parallel Node: Executes all child nodes simultaneously.
    5. Decorator Node: Modifies the behavior of its child node.
    6. Action Node: Performs a specific action when executed.
    7. Condition Node: Checks if a condition is met.

    Best Practices:
    - Keep the tree structure simple and modular.
    - Use descriptive names for nodes.
    - Implement reusable subtrees for common behaviors.
    - Balance between reactivity and goal-oriented behavior.
    - Regularly test and debug the behavior tree.
    - Document the purpose and functionality of each node.
    - Consider performance implications for large trees.
    - Use parameters to make nodes more flexible and reusable.
    - Implement logging for easier debugging and analysis.
    - Review and refactor the tree periodically to maintain clarity and efficiency.
  `;

  return (
    <div className="min-h-screen bg-gradient-to-b from-gray-100 to-gray-200 p-4">
      <h1 className="text-3xl font-bold text-center mb-6">Behavior Tree</h1>
      <Tabs defaultValue="doctrine" className="w-full max-w-4xl mx-auto">
        <TabsList className="grid w-full grid-cols-2">
          <TabsTrigger
            value="doctrine"
            onClick={() => setActiveTab("doctrine")}
          >
            Retrieved Doctrine
          </TabsTrigger>
          <TabsTrigger
            value="visualization"
            onClick={() => setActiveTab("visualization")}
          >
            Visualization
          </TabsTrigger>
        </TabsList>
        <TabsContent value="doctrine" className="mt-6">
          <ScrollArea className="h-[70vh] w-full rounded-md border p-4 bg-white">
            <pre className="whitespace-pre-wrap font-mono text-sm">
              {doctrineText}
            </pre>
          </ScrollArea>
        </TabsContent>
        <TabsContent value="visualization" className="mt-6">
          <div className="h-[70vh] w-full rounded-md border p-4 bg-white overflow-auto">
            <svg width="100%" height="100%" viewBox="0 0 800 600">
              {/* Root */}
              <g transform="translate(400,50)">
                <rect
                  x="-60"
                  y="-20"
                  width="120"
                  height="40"
                  rx="5"
                  fill="#4CAF50"
                />
                <text x="0" y="5" textAnchor="middle" fill="white">
                  Root
                </text>

                {/* Selector */}
                <g transform="translate(0,60)">
                  <rect
                    x="-60"
                    y="-20"
                    width="120"
                    height="40"
                    rx="5"
                    fill="#2196F3"
                  />
                  <text x="0" y="5" textAnchor="middle" fill="white">
                    Selector
                  </text>

                  {/* Sequence 1 */}
                  <g transform="translate(-200,60)">
                    <rect
                      x="-60"
                      y="-20"
                      width="120"
                      height="40"
                      rx="5"
                      fill="#FFC107"
                    />
                    <text x="0" y="5" textAnchor="middle" fill="white">
                      Sequence
                    </text>

                    {/* Action 1 */}
                    <g transform="translate(-70,60)">
                      <rect
                        x="-50"
                        y="-20"
                        width="100"
                        height="40"
                        rx="5"
                        fill="#9C27B0"
                      />
                      <text x="0" y="5" textAnchor="middle" fill="white">
                        Action 1
                      </text>
                    </g>

                    {/* Action 2 */}
                    <g transform="translate(70,60)">
                      <rect
                        x="-50"
                        y="-20"
                        width="100"
                        height="40"
                        rx="5"
                        fill="#9C27B0"
                      />
                      <text x="0" y="5" textAnchor="middle" fill="white">
                        Action 2
                      </text>
                    </g>
                  </g>

                  {/* Sequence 2 */}
                  <g transform="translate(200,60)">
                    <rect
                      x="-60"
                      y="-20"
                      width="120"
                      height="40"
                      rx="5"
                      fill="#FFC107"
                    />
                    <text x="0" y="5" textAnchor="middle" fill="white">
                      Sequence
                    </text>

                    {/* Condition */}
                    <g transform="translate(-70,60)">
                      <rect
                        x="-50"
                        y="-20"
                        width="100"
                        height="40"
                        rx="5"
                        fill="#FF5722"
                      />
                      <text x="0" y="5" textAnchor="middle" fill="white">
                        Condition
                      </text>
                    </g>

                    {/* Action 3 */}
                    <g transform="translate(70,60)">
                      <rect
                        x="-50"
                        y="-20"
                        width="100"
                        height="40"
                        rx="5"
                        fill="#9C27B0"
                      />
                      <text x="0" y="5" textAnchor="middle" fill="white">
                        Action 3
                      </text>
                    </g>
                  </g>
                </g>
              </g>

              {/* Connections */}
              <path d="M400,70 L400,90" stroke="black" />
              <path d="M400,130 L200,170" stroke="black" />
              <path d="M400,130 L600,170" stroke="black" />
              <path d="M200,210 L130,250" stroke="black" />
              <path d="M200,210 L270,250" stroke="black" />
              <path d="M600,210 L530,250" stroke="black" />
              <path d="M600,210 L670,250" stroke="black" />
            </svg>
          </div>
        </TabsContent>
      </Tabs>
    </div>
  );
}
