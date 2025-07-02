#pragma once

// Game Memory

#include "common.hpp"
#include "file/parser.hpp"

#define ALIGN 0xfffffffe

struct MemSlot {
    u16 m_index;
    u16 m_validator;
};

struct MemSlotInfo {
    MemSlot m_slot;
    ResourceInfo* m_info;
};

class GMem {
public:
    GMem(u32 slots, u32 specialSlots);

    static MemSlotInfo AddObject(void* obj, ResourceInfo* info);

    void SetSlot(void* obj, MemSlot* slot);

    inline void* Get(MemSlot slot) {
        return (void*)m_slots[slot.m_index];
    }

    inline bool IsValid(MemSlot slot) {
        return m_validators[slot.m_index] == slot.m_validator;
    }

private:
    void AllocSlot(void* obj, MemSlot* slot);

    void AllocSpecialSlot(void* obj, MemSlot* slot, u32 unk0, u32 unk1);

public:
    static GMem* s_instance;

private:
    u32* m_currentSlot;
    u32* m_currentSpecialSlot;
    u32* m_slots;
    u16* m_validators;
    u32* unk_10;
    u32 m_counter;
    u32 m_specialSlotCount;
    u32 m_slotCount;
};

template <typename T>
class MSlot {
public:
    inline T* Get() {
        return GMem::Get(m_slot);
    }

    inline bool IsValid() const {
        return GMem::IsValid(m_slot);
    }

    inline MemSlot Raw() const {
        return m_slot;
    }

private:
    MemSlot m_slot;
};