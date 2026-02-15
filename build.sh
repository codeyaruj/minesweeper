#!/bin/bash

# Minesweeper Build Script
# Usage: ./build.sh [clean|release|debug]

set -e

BUILD_TYPE="Release"
CLEAN=0

# Parse arguments
for arg in "$@"; do
    case $arg in
        clean)
            CLEAN=1
            ;;
        release)
            BUILD_TYPE="Release"
            ;;
        debug)
            BUILD_TYPE="Debug"
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: ./build.sh [clean|release|debug]"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Configure
echo "Configuring with CMake ($BUILD_TYPE)..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Determine number of cores
if command -v nproc > /dev/null; then
    CORES=$(nproc)
elif command -v sysctl > /dev/null; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi

# Build
echo "Building with $CORES cores..."
make -j$CORES

echo ""
echo "Build complete! Executable: build/Minesweeper"
echo ""
echo "To run: cd build && ./Minesweeper"
echo ""
