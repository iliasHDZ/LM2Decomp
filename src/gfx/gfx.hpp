#pragma once

#include "../common.hpp"
#include "registers.h"
#include <string.h>

class Gfx {
public:
    inline void AddWrite(u32 reg, u32 value) {
        m_commands[m_commandIndex + 0] = 0xf0000 | reg;
        m_commands[m_commandIndex + 1] = value;
        m_commandIndex += 2;
    }

    inline void AddWriteBuffer(const u32* buffer, u32 count) {
        memcpy(&m_commands[m_commandIndex], buffer, count * sizeof(u32));
        m_commandIndex += count;
    }

    inline Gfx* Get() {
        return s_instance;
    }

public:
    void SetAttribBuffersBaseAddr(void* addr);

    void DrawElements(void* indexArray, u32 count, u32 isByte = 0);

private:
    Gfx* s_instance;

private:
    // [...]
    u32* m_commands;
    u32  m_commandIndex;
};