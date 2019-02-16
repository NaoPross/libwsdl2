#include "wsdl2.hpp"
#include "debug.hpp"

#ifdef WRAPSDL2_EXCEPTIONS
#include <exception>
#endif

extern "C" {
#include <SDL2/SDL.h>

#ifdef IMG_LOADING
#include <SDL2/SDL_image.h>
#endif
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

#ifdef IMG_LOADING
    IMG_init(IMG_INIT_PNG | IMG_INIT_JPG);
    npdebug("png loading environment initialized")
#endif

    return true;
}

void wsdl2::quit(void) {

#ifdef IMG_LOADING
    IMG_quit();
    npdebug("png loading environment deinitialized")
#endif

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
