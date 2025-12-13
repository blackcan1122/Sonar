#!/bin/bash

# Get the directory where the script is located
base_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Prompt for the class name
read -p "Enter the class name: " classname

# Prompt for the subdirectory name
read -p "Enter the subdirectory name: " subdirname

# Define the base src directory
src_dir="$base_dir/src"

# Define public and private directories with the subdirectory name inside them
public_dir="$src_dir/public/$subdirname"
private_dir="$src_dir/private/$subdirname"

# Create directories if they don't exist
if [ ! -d "$public_dir" ]; then
    mkdir -p "$public_dir"
    echo "Created public directory: $public_dir"
fi

if [ ! -d "$private_dir" ]; then
    mkdir -p "$private_dir"
    echo "Created private directory: $private_dir"
fi

# Define file paths
header_file="$public_dir/$classname.hpp"
source_file="$private_dir/$classname.cpp"

# Create empty header and source files
touch "$header_file"
touch "$source_file"

echo "Files created successfully:"
echo "Header: $header_file"
echo "Source: $source_file"

# Make the script executable if build exists
if [ -f "$base_dir/Makefile" ]; then
    make -C "$base_dir"
fi