#!/bin/bash
# Project Verification Script

echo "=== Advanced Minesweeper Project Verification ==="
echo ""

# Check documentation
echo "📄 Documentation Files:"
ls -1 *.md | nl
echo ""

# Check build system
echo "🔨 Build System:"
ls -1 CMakeLists.txt build.* .gitignore 2>/dev/null | nl
echo ""

# Check headers
echo "📋 Header Files:"
find include -name "*.h" | nl
echo ""

# Check source files
echo "💻 Source Files:"
find src -name "*.cpp" | nl
echo ""

# Check tests
echo "🧪 Test Files:"
find tests -name "*.h" | nl
echo ""

# Count lines
echo "📊 Statistics:"
echo "Total files: $(find . -type f | wc -l)"
echo "Header files: $(find include -name '*.h' | wc -l)"
echo "Source files: $(find src -name '*.cpp' | wc -l)"
echo "Documentation: $(ls *.md 2>/dev/null | wc -l)"
echo ""

# Verify structure
echo "✅ Verification Complete!"
echo ""
echo "To build: ./build.sh release"
echo "To read: Start with START_HERE.md"
