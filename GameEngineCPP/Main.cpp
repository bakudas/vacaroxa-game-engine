#include "include/SDL.h"
#include "Game.h"
#include <cstdio>

int main(int argc, char* argv[])
{
    Game game;

    bool success = game.Initialize();

    if (success)
    {
        game.RunLoop();
    }

    game.Shutdown();

    return 0;
}