#include "memory_pool.h"
#include <stdexcept>

namespace core {
namespace utils {

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : blockSize(blockSize), blockCount(blockCount) {
    memory = std::make_unique<char[]>(blockSize * blockCount);
    initialize();
}

MemoryPool::~MemoryPool() {
    // Memory automatically freed by unique_ptr
}

void MemoryPool::initialize() {
    freeBlocks.reserve(blockCount);
    for (size_t i = 0; i < blockCount; ++i) {
        freeBlocks.push_back(memory.get() + i * blockSize);
    }
}

void* MemoryPool::allocate() {
    if (freeBlocks.empty()) {
        throw std::runtime_error("MemoryPool: No free blocks available");
    }

    void* block = freeBlocks.back();
    freeBlocks.pop_back();
    return block;
}

void MemoryPool::deallocate(void* ptr) {
    if (!ptr) {
        return;
    }

    // Simple validation: check if pointer is within our memory range
    char* charPtr = static_cast<char*>(ptr);
    char* memStart = memory.get();
    char* memEnd = memStart + (blockSize * blockCount);

    if (charPtr < memStart || charPtr >= memEnd) {
        throw std::runtime_error("MemoryPool: Invalid pointer for deallocation");
    }

    freeBlocks.push_back(ptr);
}

size_t MemoryPool::getAvailableBlocks() const {
    return freeBlocks.size();
}

} // namespace utils
} // namespace core
