#include "header.hpp"

size_t MemoryMetrics::allocatedMemory = 0;
size_t MemoryMetrics::freedMemory = 0;
size_t MemoryMetrics::trackMemoryUsage() { return allocatedMemory - freedMemory; }
void MemoryMetrics::reset() { allocatedMemory = 0; freedMemory = 0; }

static const int MAX_ALLOCATIONS = 10000;
static void* allocationPtrs[MAX_ALLOCATIONS];
static size_t allocationSizes[MAX_ALLOCATIONS];
static int allocationCount = 0;

//records a new allocation (pointer + size) in the tracking arrays
static void registerAllocation(void* ptr, size_t size) {
    if (allocationCount < MAX_ALLOCATIONS) {
        allocationPtrs[allocationCount] = ptr;
        allocationSizes[allocationCount] = size;
        allocationCount++;
    } else {
        cerr << "Memory allocation limit reached!" << endl;
    }
}

//finds and removes an allocation record, returns the size that was freed
static size_t unregisterAllocation(void* ptr) {
    for (int i = 0; i < allocationCount; ++i) {
        if (allocationPtrs[i] == ptr) {
            size_t freedSize = allocationSizes[i];
            //shift remaining entries left to fill the gap
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

//override global new — tracks every single-object allocation
void* operator new(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        registerAllocation(ptr, size);
        MemoryMetrics::allocatedMemory += size;
    }
    return ptr;
}

//override global delete — tracks every single-object deallocation
void operator delete(void* ptr) noexcept {
    if (!ptr) return;
    size_t freedSize = unregisterAllocation(ptr);
    MemoryMetrics::freedMemory += freedSize;
    free(ptr);
}

//sized delete variant (C++14)
void operator delete(void* ptr, size_t size) noexcept {
    if (!ptr) return;
    MemoryMetrics::freedMemory += size;
    free(ptr);
}

//override global new[] — tracks array allocations (e.g. new AStarNode[200])
void* operator new[](size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        registerAllocation(ptr, size);
        MemoryMetrics::allocatedMemory += size;
    }
    return ptr;
}

//override global delete[] — tracks array deallocations
void operator delete[](void* ptr) noexcept {
    if (!ptr) return;
    size_t freedSize = unregisterAllocation(ptr);
    MemoryMetrics::freedMemory += freedSize;
    free(ptr);
}

//sized delete[] variant (C++14)
void operator delete[](void* ptr, size_t size) noexcept {
    if (!ptr) return;
    MemoryMetrics::freedMemory += size;
    free(ptr);
}
