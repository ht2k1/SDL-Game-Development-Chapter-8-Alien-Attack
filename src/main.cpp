#include "Game.h"
//#include "windows.h"
#include <iostream>

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int arg, char **argv) {
    //AllocConsole();
    //freopen("CON", "w", stdout);

    Uint32 frameStart, frameTime;

    std::cout << "game init attempt...\n";
#ifdef __ANDROID__
    // fullscreen=true; SDL_WINDOW_FULLSCREEN on Android renders at the
    // device's native resolution regardless of the width/height passed
    // here - those values still define the game's *design* resolution,
    // used for all the existing gameplay math (collision bounds, level
    // width, etc.), and get stretched to fill the real screen in
    // Game::init() via SDL_RenderSetScale.
    bool initSuccess = TheGame::Instance()->init("Alien Attack", 0, 0, 640, 480, true);
#else
    bool initSuccess = TheGame::Instance()->init("Alien Attack", 100, 100, 640, 480, false);
#endif
    if (initSuccess) {
        std::cout << "game init success!\n";
        while (TheGame::Instance()->running()) {
            frameStart = SDL_GetTicks();
            
            TheGame::Instance()->handleEvents();
            TheGame::Instance()->update();
            TheGame::Instance()->render();

            frameTime = SDL_GetTicks() - frameStart;

            if (frameTime < DELAY_TIME) {
                SDL_Delay((int)(DELAY_TIME - frameTime));
            }
        }
    }
    else {
        std::cout << "game init failure - " << SDL_GetError() << "\n";
        return -1;
    }

    std::cout << "game closing...\n";
    TheGame::Instance()->clean();

    return 0;
}

