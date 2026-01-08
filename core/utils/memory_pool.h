#ifndef CORE_UTILS_MEMORY_POOL_H
#define CORE_UTILS_MEMORY_POOL_H

#include <cstddef>
#include <vector>
#include <memory>

namespace core {
namespace utils {

class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    void* allocate();
    void deallocate(void* ptr);

    size_t getBlockSize() const { return blockSize; }
    size_t getAvailableBlocks() const;

private:
    size_t blockSize;
    size_t blockCount;
    std::vector<void*> freeBlocks;
    std::unique_ptr<char[]> memory;

    void initialize();
};

} // namespace utils
} // namespace core

#endif // CORE_UTILS_MEMORY_POOL_H
