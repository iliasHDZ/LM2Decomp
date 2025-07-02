#include "allocator.hpp"
#include "stdlib.h"

Allocator Allocator::s_obInstance;

Allocator* Allocator::s_instance = &s_obInstance;

Allocator::Allocator() {}

Allocator::~Allocator() {}

void* Allocator::malloc(u32 size) {
    return ::malloc(size);
}

void Allocator::free(void* block) {
    ::free(block);
}

void* nlMalloc(u32 size) {
    return Allocator::s_instance->malloc(size);
}

void nlFree(void* block) {
    Allocator::s_instance->free(block);
}