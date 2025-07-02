#pragma once

#include "../common.hpp"

#include "../data/avlTree.hpp"
#include "resource.hpp"
#include "resourceList.hpp"

#define RESN_LABEL 0x1301

class FileCollection;

struct resource_node {
    u32 m_header;
    u32 m_size;
    u32 m_offset;

    inline u32 Type() const {
        return m_header & 0x807fffff;
    }

    inline u32 FileIndex() const {
        return (m_header & 0x70000000) >> 0x1c;
    }
};

struct label_node {
    u32 m_header;
    u32 m_name;
};

struct ResourceReader {
    u32 unk_00;
    FileCollection* m_files;
    resource_node* m_nodeTable;
    u32 m_count;
    u32 m_index;

    inline resource_node& Node() const {
        return m_nodeTable[m_index];
    }

    inline bool End() const {
        return m_index >= m_count;
    }

    inline u8* Data() const;

    inline u32 Size() const {
        return Node().m_size;
    }

    inline void Next() {
        m_index++;
    }
};

struct LoadSettings {
    u8 m_code;
};

class Resource {};

class Parser {
public:
    virtual Resource* Parse(ResourceReader* rd, label_node* label) = 0;

    // [UNK VIRTUAL FUNC 1]

    // [UNK VIRTUAL FUNC 2]

    // [UNK VIRTUAL FUNC 3]

    virtual u32 GetLoadingMethod() = 0;

    virtual u32 GetMagicNumber() = 0;

public:
    static void LoadData(nlAVLTree<u32, Parser*>* parsers, ResourceReader* rd, LoadSettings* settings, void* param);

public:
    ResourceInfo* Load(ResourceReader* rd, label_node* label, void* param);
};