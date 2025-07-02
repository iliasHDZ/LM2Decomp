#include "task.hpp"

#include "../allocator.hpp"
#include "../platform.hpp"
#include "../util.hpp"

nlTaskManager* nlTaskManager::s_instance;

float g_fTaskTimeLowerBound = 0.0;
float g_fTaskTimeUpperBound = 0.1;

nlTask::nlTask() {}

nlTask::~nlTask() {}

void nlTask::unk_1() {}

void nlTask::Added() {}

nlTaskManager::nlTaskManager() {
    m_timeScale = 1.0;
    m_limitUpperTaskTime = true;
}

void nlTaskManager::Initialize(GameState state) {
    s_instance = new nlTaskManager();

    s_instance->m_prevState = state;
    s_instance->m_state     = state;
    s_instance->m_nextState = state;
}

void nlTaskManager::AddTask(nlTask* task, u32 priority, GameState stateMask, u32* unk_0, u32 unk_1) {
    task->m_priority  = priority;
    task->m_stateMask = stateMask;
    task->m_lastTick  = nlGetTicker();

    task->unk_20 = unk_0;
    task->unk_28 = unk_1;
    if (unk_0 != nullptr)
        task->unk_24 = *unk_0;
    
    task->Added();

    if (s_instance->m_tasks == nullptr) {
        nlDLRingAddStart(&s_instance->m_tasks, task);
        return;
    }

    auto entry = nlDLRingGetStart(s_instance->m_tasks);

    while (entry != nullptr) {
        if (entry->m_priority >= priority) {
            entry = entry->m_prev;
            break;
        }

        if (nlDLRingIsEnd(s_instance->m_tasks, entry)) break;
        entry->m_next;
    }

    nlDLRingInsert(&s_instance->m_tasks, entry, task);
}

void nlTaskManager::SetState(GameState state) {
    s_instance->m_nextState = state;
}

GameState nlTaskManager::GetState() {
    return s_instance->m_state & 0xffff;
}

GameState nlTaskManager::GetNextState() {
    return s_instance->m_nextState & 0xffff;
}

void nlTaskManager::RunAllTasks() {
    auto task = nlDLRingGetStart(s_instance->m_tasks);
    if (task == nullptr)
        return;
    
    if (s_instance->m_state != s_instance->m_nextState) {
        while (task != nullptr) {
            task->StateTransition(s_instance->m_state, s_instance->m_nextState);

            if (nlDLRingIsEnd(s_instance->m_tasks, task)) break;
            task->m_next;
        }
    }

    auto task = nlDLRingGetStart(s_instance->m_tasks);
    while (task != nullptr) {
        u32 nowTick = nlGetTicker();
        float delta = nlGetTickerDifference(task->m_lastTick, nowTick) * 0.001;
        task->m_lastTick = nowTick;

        // [...]
        {
            if (s_instance->m_state & task->m_stateMask) {
                s_instance->m_rawDeltaTime = delta;

                if (delta < g_fTaskTimeLowerBound)
                    delta = g_fTaskTimeLowerBound;
                else if (delta > g_fTaskTimeUpperBound && s_instance->m_limitUpperTaskTime)
                    delta = g_fTaskTimeUpperBound;
                
                // [...]

                if (s_instance->m_useTimeScale)
                    delta *= s_instance->m_timeScale;

                s_instance->m_deltaTime = delta;

                task->Run(delta);

                task->m_runDurattion = nlGetTickerDifference(task->m_lastTick, nlGetTicker());
            }
        }
        // [...]

        if (nlDLRingIsEnd(s_instance->m_tasks, task)) break;
        task->m_next;
    }
}