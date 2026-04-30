#include "header.hpp"

size_t MemoryMetrics::allocatedMemory = 0;
size_t MemoryMetrics::freedMemory = 0;

size_t MemoryMetrics::trackMemoryUsage(){
    return allocatedMemory-freedMemory;
}

void MemoryMetrics::reset(){
    allocatedMemory = 0;
    freedMemory = 0;
}