cmake_minimum_required(VERSION 3.20)
project(HybridMediaEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_APP "Build application" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bindings/cpp_to_rust)

# Core library
add_subdirectory(core)

# Bindings to Rust
add_subdirectory(bindings/cpp_to_rust)

# Application
if(BUILD_APP)
    add_subdirectory(app)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/cpp)
endif()

# Find Rust library (will be built separately via Cargo)
# Link with compiled Rust static libraries
set(RUST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rust-modules/target/release)
link_directories(${RUST_LIB_DIR})
