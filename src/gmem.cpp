#include "gmem.hpp"
#include "allocator.hpp"

GMem::GMem(u32 slots, u32 specialSlots) {
    m_counter = 0;
    m_specialSlotCount = specialSlots;
    m_slotCount = slots;
    m_validators = new u16[slots];
    m_slots = new u32[slots];
    unk_10 = new u32[specialSlots];

    m_currentSpecialSlot = m_slots;
    m_currentSlot = m_slots + specialSlots;

    for (u32 i = 0; i < slots - 1; i++) {
        m_slots[i] = (u32)(m_slots + i + 1) | 1;
        m_validators[i] = 1;
    }

    m_slots[slots - 1] = 1;
    m_slots[specialSlots - 1] = 1;
    m_validators[slots - 1] = 1;
}

MemSlotInfo GMem::AddObject(void* obj, ResourceInfo* info) {
    MemSlotInfo ret;
    ret.m_info = info;
    s_instance->AllocSlot(obj, &ret.m_slot);
    return ret;
}

void GMem::SetSlot(void* obj, MemSlot* slot) {
    m_slots[slot->m_index] = (u32)obj & ALIGN;
}

void GMem::AllocSlot(void* obj, MemSlot* slot) {
    u32* newSlot = m_currentSlot;
    m_currentSlot = (u32*)(*m_currentSlot & ALIGN);
    u16 slotIdx = (u32)(newSlot - m_slots);
    u16 validator = m_validators[slotIdx];
    *newSlot = (u32)obj & ALIGN;

    slot->m_index = slotIdx;
    slot->m_validator = validator;
}

static u32 GetLeadingOnes(u32 v) {
    u32 ret = 0;
    do {
        v >>= 1;
        ret++;
    } while (v);
    return ret;
}

void GMem::AllocSpecialSlot(void* obj, MemSlot* slot, u32 unk0, u32 unk1) {
    u32* newSlot = m_currentSpecialSlot;
    m_currentSpecialSlot = (u32*)(*newSlot & ALIGN);
    u32 slotIdx = (u32)(newSlot - m_slots);
    slot->m_index = slotIdx;
    slot->m_validator = m_validators[slotIdx];
    *newSlot = (u32)obj & ALIGN;

    unk_10[slot->m_index] = unk0 | GetLeadingOnes(unk1) << 28;
    m_counter++;
}