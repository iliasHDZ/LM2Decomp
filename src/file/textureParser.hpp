#pragma once

#include "parser.hpp"

struct TextureInfo {
    u32 m_length;
    u32 m_hash;
    u32 m_cacheKey;
    u32 unk_0c;
    u16 m_width;
    u16 m_height;
    u16 unk_14;
    u8  unk_16;
    u8  m_mipmapLevel;
    u16 m_width2;
    u16 m_height2;
    u32 m_texParams;
    u16 m_LODBias;
    u16 m_mipmapThing;
    void* m_physicalAddr;
    u32 unk_28;
    u8  m_texType;
    u8  unk_2d;
    u8  unk_2e;
    u8  unk_2f;
};

class TextureResource : public Resource {
public:
    TextureInfo info;
};

class TextureParser {
public:
    virtual Resource* Parse(ResourceReader* rd, label_node* label);

    // [UNK VIRTUAL FUNC 1]

    // [UNK VIRTUAL FUNC 2]

    // [UNK VIRTUAL FUNC 3]

    virtual u32 GetLoadingMethod();

    virtual u32 GetMagicNumber();
};