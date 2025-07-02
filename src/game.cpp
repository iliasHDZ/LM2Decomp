#include "game.hpp"

#include "platform.hpp"
#include "task/task.hpp"

Game::Game() {
    m_shouldQuit = false;

    // [...]

    Initialize();

    // [...]

    nlTaskManager::Initialize(0);

    // [...]

    nlTaskManager::SetState(GSTATE_BOOTUP);
}

void Game::Initialize() {
    nlSetMillisecondsPerTick();

    // [...]
}

Game::~Game() {
    // TODO: RE this
}

void Game::GameLoop() {

}