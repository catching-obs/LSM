#!/bin/bash
set -e

echo "=== Building Hybrid Media Engine ==="

# Build Rust modules
echo ""
echo "Step 1: Building Rust modules..."
cd rust-modules
cargo build --release
cd ..

# Build C++ project
echo ""
echo "Step 2: Building C++ project..."
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..

echo ""
echo "=== Build completed successfully! ==="
echo ""
echo "To run tests:"
echo "  - Rust: cd rust-modules && cargo test"
echo "  - C++:  cd build && ctest"
echo ""
echo "To run the demo app:"
echo "  cd build/app && ./media_app"
