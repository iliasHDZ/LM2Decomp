#pragma once

#include "../common.hpp"

#include "../data/avlTree.hpp"

#include "../gmem.hpp"

struct ResourceSearchResult {
    nlAVLTree<u32, MemSlotInfo>::Node* m_node;
    nlAVLTree<u32, MemSlotInfo>*       m_tree;
};

struct ResourceInfo {
    Parser* m_parser;
    u32 m_name;
};

class ResourceList {
public:
    ResourceList();

public:
    int ResourceTypeFromHeader(u32 header);
    
    ResourceSearchResult Search(int type, u32 name);

    void AddResource(u32 header, u32 name, MemSlotInfo info);

    static ResourceSearchResult SearchTree(nlAVLTree<u32, MemSlotInfo>* tree, u32 name);

public:
    static ResourceList* s_instance;

private:
    nlAVLTree<u32, MemSlotInfo>* m_list;
};