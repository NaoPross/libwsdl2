#include "wrapsdl2.hpp"
#include "debug.hpp"

extern "C" {
#include <SDL2/SDL.h>
}

bool wrapsdl2::initialize(void) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        npdebug("failed to initialize sdl video subsystem");
        return false;
    }

    return true;
}

void wrapsdl2::quit(void) {
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    
    SDL_Quit();
}



void wrapsdl2::delay(unsigned ms) {
    SDL_Delay(ms);
}