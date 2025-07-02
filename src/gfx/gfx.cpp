#include "gfx.hpp"
#include "../os.hpp"

const u32 DrawElementsCommandSequence[] = {
    0x0, 0x10245, 0x1, 0xF022F,
    0x1, 0x10245, 0x1, 0xF0231,
    0x0, 0x8025E, 0x0, 0x8025E,
    0x0, 0x10253
};

void Gfx::SetAttribBuffersBaseAddr(void* addr) {
    u32 pa = OSGetPhysicalAddr(addr);
    AddWrite(GPUREG_ATTRIBBUFFERS_LOC, pa >> 3);
}

void Gfx::DrawElements(void* indexArray, u32 count, u32 isByte = 0) {
    AddWrite(GPUREG_INDEXBUFFER_CONFIG, (u32)indexArray | (isByte ? 0 : 0x80000000));
    AddWrite(GPUREG_NUMVERTICES, count);
    AddWriteBuffer(DrawElementsCommandSequence, sizeof(DrawElementsCommandSequence) / sizeof(u32));
}