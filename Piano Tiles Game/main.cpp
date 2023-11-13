#include "Game/Game.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
    FreeConsole();
    
    Game game;

    while (!game.GetExitStatus())
    {
        game.RunMainMenu();
    
        game.SetFailedStatus(false);
        game.RunGame();   
    }
    
    SDL_RenderClear(game.GetRenderer());
    SDL_RenderPresent(game.GetRenderer());
    
    return 0;
}