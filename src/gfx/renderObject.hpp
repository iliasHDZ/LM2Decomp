#pragma once

#include "../common.hpp"

struct RenderObject {
    short* m_indexArray;
    u32 m_vertexCount;
    void** m_vertexBuffer; // This points to a pointer, which points to the actual vertex buffer
};