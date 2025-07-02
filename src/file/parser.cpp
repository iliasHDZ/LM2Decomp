#include "parser.hpp"
#include "dictFile.hpp"
#include "../gmem.hpp"

inline u8* ResourceReader::Data() const {
    return m_files->m_datas[Node().FileIndex()] + Node().m_offset;
}

void Parser::LoadData(nlAVLTree<u32, Parser*>* parsers, ResourceReader* rd, LoadSettings* settings, void* param) {
    while (!rd->End()) {
        label_node* label = nullptr;

        if (rd->Node().Type() == RESN_LABEL) {
            label = (label_node*)rd->Data();
            if (!rd->End())
                rd->Next();
        }

        if (settings->m_code == 0) {
            Parser* parser = nullptr;
            auto node = parsers->Find(rd->Node().Type());
            if (node) parser = node->value;

            parser->Load(rd, label, param);

            // [...]
        }

        // [...]
        rd->Next();
    }

    // [...]
}

ResourceInfo* Parser::Load(ResourceReader* rd, label_node* label, void* param) {
    // Not sure what type this is yet
    MemSlotInfo* resource = nullptr;

    if (label) {
        u32 type = ResourceList::s_instance->ResourceTypeFromHeader(label->m_header);
        auto result = ResourceList::s_instance->Search(type, label->m_name);

        if (result.m_node != nullptr && result.m_tree != nullptr)
            resource = &(result.m_node->value);
    }

    ResourceInfo* info;

    switch (GetLoadingMethod()) {
    
    // [...]
    
    case 3:
        // [...]
        Resource* res = Parse(rd, label);

        info = new ResourceInfo();
        info->m_parser = this;
        info->m_name   = label->m_name;

        MemSlotInfo slot = GMem::AddObject(res, info);
        ResourceList::s_instance->AddResource(label->m_header, info->m_name, slot);
        break;
    };
    
    // [...]

    return info;
}