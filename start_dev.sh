#!/bin/bash

# Function to log messages with timestamps
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Kill processes on script exit
cleanup() {
    log_message "Cleaning up..."
    if [ ! -z "$API_PID" ]; then
        kill $API_PID 2>/dev/null || true
    fi
    if [ ! -z "$VITE_PID" ]; then
        kill $VITE_PID 2>/dev/null || true
    fi
}

trap cleanup EXIT

# Start FastAPI server
log_message "Starting FastAPI server..."
cd codellama-bt-adapter
python3 -m uvicorn api.main:app --reload --host 0.0.0.0 --port 8000 &
API_PID=$!

# Wait a bit for API server to start
sleep 2

# Start Vite development server
log_message "Starting Vite development server..."
cd ../bt-viewer
npm run dev &
VITE_PID=$!

# Keep script running and show logs
log_message "Development environment started"
log_message "API Server running on http://localhost:8000"
log_message "Vite Server running on http://localhost:5173"
log_message "Press Ctrl+C to stop all servers"

# Wait for Ctrl+C
wait