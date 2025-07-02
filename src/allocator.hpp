#pragma once

#include "common.hpp"

class Allocator {
public:
    Allocator();

    virtual ~Allocator();

    virtual void* malloc(u32 size);

    virtual void free(void* block);

    static Allocator* get();

private:
    static Allocator s_obInstance;

public:
    static Allocator* s_instance = &s_obInstance;

};

void* nlMalloc(u32 size);

void nlFree(void* block);

void* operator new(size_t size) {
    return nlMalloc(size);
}
 
void operator delete(void* p) {
    nlFree(p);
}

void* operator new[](size_t size) {
    return nlMalloc(size);
}
 
void operator delete[](void* p) {
    nlFree(p);
}