#pragma once

#include "common.hpp"
#include "../data/linkedList.hpp"

#define GSTATE_BOOTUP                       0x0001
#define GSTATE_START_FRONTEND               0x0002
#define GSTATE_FRONTEND                     0x0004
#define GSTATE_SHUTDOWN_FRONTEND            0x0008
#define GSTATE_START_GAMEPLAY               0x0010
#define GSTATE_GAMEPLAY                     0x0020
#define GSTATE_SHUTDOWN_GAMEPLAY            0x0040
#define GSTATE_PAUSED                       0x0080
#define GSTATE_NIS                          0x0100
#define GSTATE_START_MULTIPLAYER_STATS      0x0200
#define GSTATE_MULTIPLAYER_STATS            0x0400
#define GSTATE_SHUTDOWN_MULTIPLAYER_STATS   0x0800
#define GSTATE_EXIT_APP                     0x8000
#define GSTATE_ALL                          0xffff

typedef u32 GameState;

class nlTask {
public:
    nlTask();

    virtual ~nlTask();

    virtual void unk_1();

    virtual void Added();

    virtual void Run(float delta) = 0;

    virtual const char* GetName() = 0;

    virtual void StateTransition(u32 oldState, u32 newState) = 0;

public:
    DLListEntry<nlTask> m_next;
    DLListEntry<nlTask> m_prev;

    u32 m_priority;
    GameState m_stateMask;
    u32 m_lastTick;
    float m_runDurattion;
    u32 unk_1c;
    u32* unk_20;
    u32 unk_24;
    u32 unk_28;
};

class nlTaskManager {
public:
    nlTaskManager();

public:
    static void Initialize(GameState state);

    static void AddTask(nlTask* task, u32 priority, GameState stateMask, u32* unk_0, u32 unk_1);

    static void SetState(GameState state);

    static GameState GetState();

    static GameState GetNextState();

    static void RunAllTasks();

public:
    float m_timeScale;
    DLListEntry<nlTask> m_tasks;

    GameState m_state;
    GameState m_nextState;
    GameState m_prevState;

    float m_deltaTime;

    float m_rawDeltaTime;
    bool m_useTimeScale;

    bool m_limitUpperTaskTime;

public:
    static nlTaskManager* s_instance;
};