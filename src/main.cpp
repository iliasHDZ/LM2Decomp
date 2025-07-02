#include "game.hpp"

int main() {
    Game* game = new Game();

    game->GameLoop();

    delete game;

    return 0;
}