#!/bin/bash
# Automated structure fix script

cd "$(dirname "$0")"

echo "=== Fixing Minesweeper Project Structure ==="
echo ""

# Create include directories if they don't exist
mkdir -p include/core include/renderer

# Move headers to correct locations
if [ -f "core/Application.h" ]; then
    mv core/Application.h include/core/
    echo "✅ Moved Application.h to include/core/"
fi

if [ -f "renderer/Renderer.h" ]; then
    mv renderer/Renderer.h include/renderer/
    echo "✅ Moved Renderer.h to include/renderer/"
fi

# Clean up empty directories
if [ -d "core" ]; then
    rmdir core 2>/dev/null && echo "✅ Removed empty core/ directory"
fi

if [ -d "renderer" ]; then
    rmdir renderer 2>/dev/null && echo "✅ Removed empty renderer/ directory"
fi

echo ""
echo "=== Structure Fixed! ==="
echo ""
echo "Now rebuild with:"
echo "  rm -rf build"
echo "  ./build.sh release"
echo ""
echo "Or manually:"
echo "  rm -rf build"
echo "  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release"
echo "  cmake --build build --parallel"
echo ""
