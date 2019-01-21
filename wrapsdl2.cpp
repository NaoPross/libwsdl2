#include "wrapsdl2.hpp"
#include "debug.hpp"

#ifdef WRAPSDL2_EXCEPTIONS
#include <exception>
#endif

extern "C" {
#include <SDL2/SDL.h>
}

bool wrapsdl2::initialize(void) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
#ifdef WRAPSDL2_EXCEPTIONS
        throw std::runtime_error("failed to initialize sdl video subsystem");
#endif
        return false;
    }

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
#ifdef WRAPSDL2_EXCEPTIONS
        throw std::runtime_error("failed to initialzie sdl events subsystem");
#endif
        return false;
    }

    npdebug("initialized sdl2");

    return true;
}

void wrapsdl2::quit(void) {
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);

    if (SDL_WasInit(SDL_INIT_EVENTS))
        SDL_QuitSubSystem(SDL_INIT_EVENTS);

    SDL_Quit();

    npdebug("deinitialized (quit) sdl2");
}



void wrapsdl2::delay(unsigned ms) {
    SDL_Delay(ms);
}