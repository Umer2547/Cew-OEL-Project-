#!/bin/bash

# Change directory to where the executable is located
cd /home/umeriqbal/Downloads/CEW\ OEL

# Check if a city name is provided as an argument
if [ -z "$1" ]; then
  echo "Usage: ./run_weather_app.sh <city_name>"
  exit 1
fi

# Get the city name from the script argument
CITY_NAME="$1"

# Infinite loop to keep the script running
while true
do
  # Run the program with the specified city
  ./weather_app "$CITY_NAME" 2>&1

  # Sleep for 600 seconds (10 minutes)
  sleep 600
done
