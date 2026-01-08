#include <iostream>
#include <cassert>
#include "utils/logger.h"
#include "utils/memory_pool.h"

using namespace core::utils;

void test_logger() {
    std::cout << "Testing Logger..." << std::endl;

    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::DEBUG);

    logger.log(LogLevel::DEBUG, "Debug message test");
    logger.log(LogLevel::INFO, "Info message test");
    logger.log(LogLevel::WARNING, "Warning message test");
    logger.log(LogLevel::ERROR, "Error message test");

    std::cout << "Logger test passed!" << std::endl;
}

void test_memory_pool() {
    std::cout << "\nTesting MemoryPool..." << std::endl;

    const size_t blockSize = 64;
    const size_t blockCount = 10;

    MemoryPool pool(blockSize, blockCount);

    // Test initial state
    assert(pool.getBlockSize() == blockSize);
    assert(pool.getAvailableBlocks() == blockCount);

    // Allocate some blocks
    void* block1 = pool.allocate();
    void* block2 = pool.allocate();
    assert(block1 != nullptr);
    assert(block2 != nullptr);
    assert(block1 != block2);
    assert(pool.getAvailableBlocks() == blockCount - 2);

    // Deallocate
    pool.deallocate(block1);
    assert(pool.getAvailableBlocks() == blockCount - 1);

    pool.deallocate(block2);
    assert(pool.getAvailableBlocks() == blockCount);

    // Test allocation exhaustion
    std::vector<void*> blocks;
    for (size_t i = 0; i < blockCount; ++i) {
        blocks.push_back(pool.allocate());
    }
    assert(pool.getAvailableBlocks() == 0);

    try {
        pool.allocate();  // Should throw
        assert(false && "Expected exception not thrown");
    } catch (const std::runtime_error&) {
        // Expected
    }

    // Clean up
    for (void* block : blocks) {
        pool.deallocate(block);
    }
    assert(pool.getAvailableBlocks() == blockCount);

    std::cout << "MemoryPool test passed!" << std::endl;
}

int main() {
    try {
        test_logger();
        test_memory_pool();

        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
