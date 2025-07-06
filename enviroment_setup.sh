#!/bin/bash


# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Script is located at: $SCRIPT_DIR"
# Check if the script is being sourced
if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
    echo "Script is being sourced."
else
    echo "Script is being executed directly."
fi
# Add a directory to the PATH in .bashrc
# This script appends a line to the .bashrc file to include a custom directory in
# the PATH environment variable, ensuring it does not duplicate existing entries.

# Directory to add
DIR_TO_ADD="$HOME/my/custom/bin"

# Text to append
LINE_TO_ADD="export PATH=\"\$PATH:$DIR_TO_ADD\""

# Check if line already exists to avoid duplicates
if ! grep -Fxq "$LINE_TO_ADD" ~/.bashrc; then
    echo "$LINE_TO_ADD" >> ~/.bashrc
    echo "Added to .bashrc: $LINE_TO_ADD"
else
    echo "Line already exists in .bashrc"
fi
