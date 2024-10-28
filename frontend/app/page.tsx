"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { useRouter } from "next/navigation";
import {
  Card,
  CardHeader,
  CardTitle,
  CardContent,
  CardFooter,
} from "@/components/ui/card";
import { Loader2 } from "lucide-react";

export default function Component() {
  const [name, setName] = useState("");
  const [parameters, setParameters] = useState("");
  const [isLoading, setIsLoading] = useState(false);
  const router = useRouter();
  const transform = (name, parameters) => {
    return {
      scenarios: [
        {
          name: name,
          prompt: parameters,
        },
      ],
    };
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setIsLoading(true);

    try {
      const response = await fetch("/api/write", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(transform(name, parameters)), // Send your data here
      });

      const result = await response.json();
      if (response.ok) {
        console.log(result.message); // Data written successfully
      } else {
        console.error("Error:", result.message); // Handle error
      }
    } catch (error) {
      console.error("Error:", error);
    }

    const response = await fetch("/api/call", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
    });

    if (response.ok) {
      const data = await response.json();
      console.log("Server response:", data);
    } else {
      console.error("Error calling API:", response.statusText);
    }

    setIsLoading(false);
    router.push("/bt");
  };

  return (
    <div className="min-h-screen flex flex-col items-center justify-center bg-gradient-to-b from-gray-100 to-gray-200 p-4 relative">
      <Card className="w-full max-w-md">
        <CardHeader>
          <CardTitle className="text-2xl font-bold text-center">
            Behavior Tree Generation
          </CardTitle>
        </CardHeader>
        <CardContent>
          <form onSubmit={handleSubmit} className="space-y-4">
            <div className="space-y-2">
              <Label htmlFor="name">Name</Label>
              <Input
                id="name"
                type="text"
                placeholder="Enter your name..."
                value={name}
                onChange={(e) => setName(e.target.value)}
                disabled={isLoading}
              />
            </div>
            <div className="space-y-2">
              <Label htmlFor="parameters">Behavior Tree Parameters</Label>
              <Input
                id="parameters"
                type="text"
                placeholder="Enter behavior tree parameters..."
                value={parameters}
                onChange={(e) => setParameters(e.target.value)}
                disabled={isLoading}
              />
            </div>
          </form>
        </CardContent>
        <CardFooter>
          <Button
            type="submit"
            className="w-full"
            onClick={handleSubmit}
            disabled={isLoading}
          >
            {isLoading ? "Generating..." : "Submit"}
          </Button>
        </CardFooter>
      </Card>

      {isLoading && (
        <div className="absolute inset-0 bg-black bg-opacity-50 flex items-center justify-center">
          <div className="bg-white rounded-lg p-8 flex flex-col items-center">
            <Loader2 className="h-16 w-16 animate-spin text-primary" />
            <p className="mt-4 text-lg font-semibold text-primary">
              Generating Behavior Tree...
            </p>
            <div className="mt-4 w-64 h-2 bg-gray-200 rounded-full overflow-hidden">
              <div
                className="h-full bg-primary animate-pulse"
                style={{ width: "60%" }}
              ></div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
