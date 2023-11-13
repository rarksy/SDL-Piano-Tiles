#include "Game.h"

#include <iostream>
#include <random>
#include <string>
#include <sstream>

#include "SDL_ttf.h"

Game::Game()
{
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        throw std::runtime_error("SDL_Init Failed!");

    if (TTF_Init() == -1)
        throw std::runtime_error("TTF_Init Failed!");

    // Create SDL window
    this->window = SDL_CreateWindow("SDL Piano Tiles",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    screenWidth,
                                    screenHeight,
                                    SDL_WINDOW_SHOWN);

    if (!this->window)
        throw std::runtime_error("window == nullptr!");

    // Create SDL renderer
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);

    if (!this->renderer)
        throw std::runtime_error("renderer == nullptr!");

    this->font = TTF_OpenFont("C:\\Windows\\Fonts\\verdana.ttf", 50);

    if (!font)
        throw std::runtime_error("Failed To Load Font!");

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(this->renderer);
    SDL_RenderPresent(this->renderer);
}

Game::~Game()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void RenderText(TTF_Font* font, SDL_Renderer* renderer, SDL_Color color, const std::string& text, int x, int y)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface)
    {
        std::cerr << "Failed to render text. TTF Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture)
    {
        std::cerr << "Failed to create texture from surface. SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

void Game::RunMainMenu()
{
    SDL_Event gameEvent;
    const int frameDelayMS = 1000 / this->GetFrameRate();
    Uint32 frameStart, frameTime;

    SDL_SetWindowTitle(this->window, "Piano Tiles!");

    this->startGame = false;


    while (!this->startGame && !this->exitGame)
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&gameEvent))
        {
            if (gameEvent.type == SDL_QUIT)
            {
                std::cout << "Exit Triggered!" << std::endl;
                this->exitGame = true;
            }
            if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if (gameEvent.button.button == SDL_BUTTON_LEFT)
                    this->startGame = true;
            }

            if (gameEvent.type == SDL_KEYDOWN)
            {
                if (gameEvent.key.keysym.scancode == SDL_SCANCODE_UP)
                    this->maxTiles++;

                if (gameEvent.key.keysym.scancode == SDL_SCANCODE_DOWN)
                    this->maxTiles--;

                if (gameEvent.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    this->maxRows++;

                if (gameEvent.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    this->maxRows--;

                if (gameEvent.key.keysym.scancode == SDL_SCANCODE_L)
                    this->lockCursor = !this->lockCursor;
            }
        }

        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
        SDL_RenderClear(this->renderer);


        RenderText(font, this->renderer, {255, 255, 255, 255}, "SDL Piano Tiles!", 70, 50);
        RenderText(font, this->renderer, {255, 255, 255, 255}, "Click To Play :)", 80, 120);
        RenderText(font, this->renderer, {255, 255, 255, 255},
                   std::string("Up/Down: Tiles: " + std::to_string(this->maxTiles)), 20, 400);
        RenderText(font, this->renderer, {255, 255, 255, 255},
                   std::string("Left/Right: Rows: " + std::to_string(this->maxRows)), 20, 450);
        RenderText(font, this->renderer, {255, 255, 255, 255},
                   std::string("L: Lock Cursor: ") + std::string(this->lockCursor ? "On" : "Off"), 20, 500);

        std::string lastScoreText = "Last Score: " + std::to_string(this->lastScore);
        RenderText(font, this->renderer, {255, 255, 255, 255}, lastScoreText.c_str(), 10, 840);
        std::string highScoreText = "High Score: " + std::to_string(this->highScore);
        RenderText(font, this->renderer, {255, 255, 255, 255}, highScoreText.c_str(), 10, 890);


        SDL_RenderPresent(this->renderer);
    }
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void Game::RunGame()
{
    SDL_Event gameEvent;
    const int frameDelayMS = 1000 / this->GetFrameRate();
    Uint32 frameStart, frameTime;

    this->tiles.clear();
    this->scrollSpeed = defaultScrollSpeed;

    if (this->lockCursor)
        SDL_SetWindowGrab(window, SDL_TRUE);

    while (!this->failed && this->startGame)
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&gameEvent))
        {
            if (gameEvent.type == SDL_QUIT)
            {
                std::cout << "Exit Triggered!" << std::endl;
                this->startGame = false;
                this->exitGame = true;
            }
            if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if (gameEvent.button.button != SDL_BUTTON_LEFT)
                    continue;

                bool clickedTile = false;
                for (auto& tile : this->tiles)
                {
                    if (MouseOverTile(tile))
                        clickedTile = true;
                }

                if (!clickedTile)
                    this->failed = true;

                for (auto& tile : this->GetTiles())
                {
                    if (!this->MouseOverTile(tile))
                        continue;

                    if (!tile.targetTile)
                    {
                        // user clicked an incorrect but VALID tile
                        this->failed = true;
                        break;
                    }

                    tile.color.a -= 100;
                    tile.clicked = true;
                    std::cout << "Tile Clicked!" << std::endl;

                    this->points++;
                }
            }
        }

        this->CreateTiles();
        
        for (auto& tile : this->GetTiles())
        {
            this->MoveTile(tile);
        }

        SDL_SetRenderDrawColor(this->GetRenderer(), 255, 255, 255, 255);
        SDL_RenderClear(this->GetRenderer());


        int rowWidth = screenWidth / this->maxRows;
        int tileHeight = screenHeight / this->maxTiles;

        SDL_SetRenderDrawColor(this->GetRenderer(), 50, 50, 50, 100);
        for (int i = 0; i < this->maxRows; ++i)
        {
            SDL_RenderDrawLine(this->renderer, (i * rowWidth), this->screenHeight, (i * rowWidth), 0);
            SDL_RenderDrawLine(this->renderer, 0, (i * tileHeight), this->screenWidth, (i * tileHeight));
        }

        for (auto& tile : this->GetTiles())
        {
            this->RenderTile(tile);
        }

        SDL_RenderPresent(this->GetRenderer());

        frameTime = SDL_GetTicks();

        if (frameDelayMS > frameTime)
            SDL_Delay(frameDelayMS - frameTime);

        this->scrollSpeed += 0.001F;

        SDL_SetWindowTitle(this->window,
                           std::string(
                               "Piano Tiles! | Score: " + std::to_string(this->points) + " | Speed: " +
                               to_string_with_precision(this->scrollSpeed, 2)).c_str());

        if (this->failed)
        {
            if (this->points > this->highScore)
                this->highScore = this->points;

            this->lastScore = points;
            this->points = 0;
        }
    }

    if (this->lockCursor)
        SDL_SetWindowGrab(window, SDL_FALSE);
}

void Game::CreateNewTile(Tile& tile)
{
    int screenWidth, screenHeight;
    SDL_GetWindowSize(this->window, &screenWidth, &screenHeight);

    int rowWidth = screenWidth / this->maxRows;
    int tileHeight = screenHeight / this->maxTiles;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> rowDistribution(0, this->maxRows - 1);

    int row = rowDistribution(gen);

    tile.width = rowWidth;
    tile.height = tileHeight;
    tile.color = this->tileColor;
    tile.x = row * rowWidth;

    this->tiles.push_back(tile);
}

void Game::CreateTiles()
{
    int screenHeight;
    SDL_GetWindowSize(this->window, nullptr, &screenHeight);

    const int tileHeight = screenHeight / this->maxTiles;

    if (this->tiles.size() == 0)
    {
        for (int i = this->tiles.size(); i < (this->maxRows + 1); ++i)
        {
            Tile tile;
            tile.y = screenHeight - ((i + 1 + this->maxTiles) * tileHeight);

            CreateNewTile(tile);
        }
    }

    for (int i = this->tiles.size(); i < (this->maxRows + 1); ++i)
    {
        Tile tile;
        tile.y = screenHeight - ((i + 1) * tileHeight);

        CreateNewTile(tile);
    }
}

void Game::RenderTile(const Tile& tile)
{
    SDL_SetRenderDrawColor(this->renderer, tile.color.r, tile.color.g, tile.color.b, tile.color.a);
    SDL_Rect rect = {(int)tile.x, (int)tile.y, (int)tile.width, (int)tile.height};
    SDL_RenderFillRect(this->renderer, &rect);
}

void Game::MoveTile(Tile& tile)
{
    Tile* targetTile = nullptr;

    for (auto& tile : this->tiles)
    {
        if (!tile.clicked && (targetTile == nullptr || tile.y > targetTile->y))
            targetTile = &tile;
    }

    for (auto& tile : tiles)
    {
        tile.targetTile = (targetTile != nullptr && &tile == targetTile);
    }

    tile.y += this->scrollSpeed;

    int width, height;
    SDL_GetWindowSize(this->window, &width, &height);

    if (tile.y > height)
    {
        auto shouldRemove = [height](const Tile& t) { return t.y > height; };

        this->tiles.erase(std::remove_if(this->tiles.begin(), this->tiles.end(), shouldRemove), this->tiles.end());

        if (!tile.clicked)
            this->failed = true;
    }
}

bool Game::MouseOverTile(const Tile& tile)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseX >= tile.x && mouseX <= (tile.x + tile.width) &&
        mouseY >= tile.y && mouseY <= (tile.y + tile.height))
        return true;

    return false;
}
