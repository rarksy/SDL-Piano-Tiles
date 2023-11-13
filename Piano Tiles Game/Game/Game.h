#pragma once
#include <SDL_ttf.h>

#include "SDL.h"
#include <vector>

struct Tile
{
    float x, y, width, height;
    SDL_Color color;
    bool clicked = false;
    bool targetTile = false;
};
//
class Game
{
public:
    Game();
    ~Game();

    void RunMainMenu();
    void RunGame();
    
    void CreateNewTile(Tile& tile);
    void CreateTiles();
    void RenderTile(const Tile& tile);
    void MoveTile(Tile& tile);
    bool MouseOverTile(const Tile& tile);

    int GetFrameRate() const
    {
        return this->frameRate;
    }

    std::vector<Tile>& GetTiles()
    {
        return this->tiles;
    }

    SDL_Renderer* GetRenderer()
    {
        return this->renderer;
    }

    bool GetExitStatus() const
    {
        return this->exitGame;
    }

    bool GetFailedStatus() const
    {
        return this->failed;
    }

    void SetFailedStatus(bool status)
    {
        this->failed = status;
    }


private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    
    std::vector<Tile> tiles;
    SDL_Color tileColor{0, 0, 0, 255};

    int points = 0;
    int highScore = 0;
    int lastScore = 0;

    int screenWidth = 540;
    int screenHeight = 960;
    int frameRate = 60;
    
    int maxTiles = 4;
    int maxRows = 4;
    bool lockCursor = false;
    
    float defaultScrollSpeed = 2;
    float scrollSpeed = defaultScrollSpeed;
    

    bool startGame;
    bool failed;
    bool exitGame;
};