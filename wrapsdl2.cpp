#include "wsdl2.hpp"
#include "debug.hpp"

#ifdef WRAPSDL2_EXCEPTIONS
#include <exception>
#endif

extern "C" {
#include <SDL2/SDL.h>
}

bool wsdl2::initialize(void) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("failed to initialize SDL video subsystem");
        return false;
    }

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
        throw std::runtime_error("failed to initialzie SDL events subsystem");
        return false;
    }

    npdebug("initialized SDL2");

    return true;
}

void wsdl2::quit(void) {
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);

    if (SDL_WasInit(SDL_INIT_EVENTS))
        SDL_QuitSubSystem(SDL_INIT_EVENTS);

    SDL_Quit();

    npdebug("deinitialized (quit) SDL2");
}



void wsdl2::delay(unsigned ms) {
    SDL_Delay(ms);
}