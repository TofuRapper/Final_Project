#include "Game.h"
#include <ctime>
#include <cstdlib>

int main() {
    srand(time(NULL));
    Game game;
    if (game.init()) {
        game.run();
    }
    game.cleanup();
    return 0;
}
