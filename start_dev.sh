#!/bin/bash

# Executive Summary:
# This script is designed to start a development environment for the Behavior Tree Viewer application.
# It starts both the FastAPI server and the Vite development server, and logs messages with timestamps.
# The script also includes a cleanup function to kill the servers when the script exits.

# Function to log messages with timestamps
log_message() {
    # This function logs messages with timestamps to the console.
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Kill processes on script exit
cleanup() {
    # This function is called when the script exits to clean up any running processes.
    log_message "Cleaning up..."
    # Check if the API_PID variable is not empty and kill the process if it is running.
    if [ ! -z "$API_PID" ]; then
        kill $API_PID 2>/dev/null || true
    fi
    # Check if the VITE_PID variable is not empty and kill the process if it is running.
    if [ ! -z "$VITE_PID" ]; then
        kill $VITE_PID 2>/dev/null || true
    fi
}

# Set up a trap to call the cleanup function when the script exits.
trap cleanup EXIT

# Start FastAPI server
log_message "Starting FastAPI server..."
# Change directory to the FastAPI server location.
cd codellama-bt-adapter
# Start the FastAPI server in the background and capture its process ID.
python3 -m uvicorn api.main:app --reload --host 0.0.0.0 --port 8000 &
API_PID=$!

# Wait a bit for API server to start
sleep 2

# Start Vite development server
log_message "Starting Vite development server..."
# Change directory to the Vite server location.
cd ../bt-viewer
# Start the Vite server in the background and capture its process ID.
npm run dev &
VITE_PID=$!

# Log messages to indicate the development environment is running.
log_message "Development environment started"
log_message "API Server running on http://localhost:8000"
log_message "Vite Server running on http://localhost:5173"
log_message "Press Ctrl+C to stop all servers"

# Wait for Ctrl+C
wait