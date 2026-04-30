#include "header.hpp"

static const int MAX_ALLOCATIONS = 1024;
static void* allocationPtrs[MAX_ALLOCATIONS];
static size_t allocationSizes[MAX_ALLOCATIONS];
static int allocationCount = 0;

static void registerAllocation(void* ptr, size_t size) {
    if (allocationCount < MAX_ALLOCATIONS) {
        allocationPtrs[allocationCount] = ptr;
        allocationSizes[allocationCount] = size;
        allocationCount++;
    }
    else {
        cerr << "Memory allocation limit reached!" << endl;
    }
}

static size_t unregisterAllocation(void* ptr) {
    for (int i = 0; i < allocationCount; ++i) {
        if (allocationPtrs[i] == ptr) {
            size_t freedSize = allocationSizes[i];
            allocationCount--;
            for (int j = i; j < allocationCount; ++j) {
                allocationPtrs[j] = allocationPtrs[j + 1];
                allocationSizes[j] = allocationSizes[j + 1];
            }
            return freedSize;
        }
    }
    return 0;
}

void* operator new(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        registerAllocation(ptr, size);
        MemoryMetrics::allocatedMemory += size;
    }
    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (!ptr) return;
    size_t freedSize = unregisterAllocation(ptr);
    MemoryMetrics::freedMemory += freedSize;
    free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept {
    if (!ptr) return;
    MemoryMetrics::freedMemory += size;
    free(ptr);
}

void* operator new[](size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        registerAllocation(ptr, size);
        MemoryMetrics::allocatedMemory += size;
    }
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    if (!ptr) return;
    size_t freedSize = unregisterAllocation(ptr);
    MemoryMetrics::freedMemory += freedSize;
    free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept {
    if (!ptr) return;
    MemoryMetrics::freedMemory += size;
    free(ptr);
}

