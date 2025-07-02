#pragma once

#include "common.hpp"

class Game {
public:
    Game();

    virtual ~Game();

    void GameLoop();

public:
    static void Initialize();

public:
    bool m_shouldQuit;

public:
    static Game* s_instance;
};