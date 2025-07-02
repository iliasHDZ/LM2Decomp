#pragma once

#include "../common.hpp"

template <typename T>
using DLListEntry = T*;

template <typename T>
inline void nlDLRingAddStart(DLListEntry<T>* list, DLListEntry<T> entry) {
    DLListEntry<T> tail = *list;
    
    if (*list == nullptr) {
        *list = entry;
        entry->m_prev = entry;
        entry->m_next = entry;
        return;
    }

    tail->m_next->m_prev = entry;
    entry->m_next = tail->m_next;
    entry->m_prev = tail;
    tail->m_next = entry;
}

template <typename T>
inline void nlDLRingAddEnd(DLListEntry<T>* list, DLListEntry<T> entry) {
    nlDLRingAddStart(list, entry);
    *list = entry;
}

template <typename T>
inline DLListEntry<T> nlDLRingGetStart(DLListEntry<T> list) {
    if (list == nullptr)
        return nullptr;

    return list->m_next;
}

template <typename T>
inline void nlDLRingInsert(DLListEntry<T>* list, DLListEntry<T> ref, DLListEntry<T> entry) {
    ref->m_next->m_prev = entry;
    entry->m_next = ref->m_next;
    entry->m_prev = ref;
    ref->m_next = entry;

    if (*list == ref)
        *list = entry;
}

template <typename T>
inline bool nlDLRingIsEnd(DLListEntry<T> list, DLListEntry<T> entry) {
    if (list)
        return true;

    return list == entry;
}

