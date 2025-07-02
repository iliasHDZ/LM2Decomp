#include "resourceList.hpp"

u32 resourceHeaders[] = {
         0x975,    0x14E94,   0x24FA24,   0x27BCF6,
      0x2E519C,  0x4E90C0F,  0x59DDB30,  0x6B7595B,
     0x785CFCC,  0xD85908F, 0x1E81B1B3, 0x2C38ABAC,
    0x3C035386, 0x44F23664, 0x7B2193E5, 0x809F6F07,
    0x97568FDC, 0xA01B1590, 0xA01B4CE6, 0xA10D4468,
    0xA60E37AC, 0xBC9FB162, 0xC686CE34, 0xC92154DB,
    0xDAD3802F, 0xE977D350, 0xF52A6A43, 0xFE7BE6FB
};

ResourceList::ResourceList() {
    m_list = new nlAVLTree<u32, MemSlotInfo>[28];
}

int ResourceList::ResourceTypeFromHeader(u32 header) {
    // In reality, it uses a binary search algorithm but I couldn't care less
    for (u32 i = 0; i < sizeof(resourceHeaders) / 4; i++)
        if (resourceHeaders[i] == header)
            return i;
    
    return -1;
}

ResourceSearchResult ResourceList::Search(int type, u32 name) {
    if (type == -1)
        return { nullptr, nullptr };
    
    auto tree = &m_list[type];
    if (tree == nullptr)
        return { nullptr, nullptr };

    return SearchTree(tree, name);
}

void ResourceList::AddResource(u32 header, u32 name, MemSlotInfo info) {
    int type = ResourceTypeFromHeader(header);

    nlAVLTree<u32, MemSlotInfo>* tree = nullptr;
    if (type != -1)
        tree = &m_list[type];
    
    tree->AddNode(name, info);
}

ResourceSearchResult ResourceList::SearchTree(nlAVLTree<u32, MemSlotInfo>* tree, u32 name) {
    auto node = tree->Find(name);
    if (node == nullptr)
        return { nullptr, nullptr };
    
    return { node, tree };
}