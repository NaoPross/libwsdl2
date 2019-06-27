#include "wsdl2/wsdl2.hpp"
#include "wsdl2/debug.hpp"
#include "wsdl2/util.hpp"

extern "C" {
#include <SDL2/SDL.h>

#ifdef WSDL2_IMG
#include <SDL2/SDL_image.h>
#endif

#ifdef WSDL2_TTF
#include <SDL2/SDL_ttf.h>
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
    npdebug("initialize SDL2_Img for PNG and JPG");
#endif

#ifdef WSDL2_TTF
    if (util::check_ttf(TTF_Init() == -1)) {
        throw std::runtime_error("failed to initialize SDL2_ttf");
    }
    npdebug("initialized SDL2_ttf");
#endif

    return true;
}

void wsdl2::quit(void) {

#ifdef WSDL2_TTF
    TTF_Quit();
    npdebug("deinitialized SDL2_ttf");
#endif

#ifdef WSDL2_IMG
    IMG_Quit();
    npdebug("deinitialized SDL2_image");
#endif

    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);

    if (SDL_WasInit(SDL_INIT_EVENTS))
        SDL_QuitSubSystem(SDL_INIT_EVENTS);

    SDL_Quit();
    npdebug("deinitialized SDL2");
}



void wsdl2::delay(unsigned ms) {
    SDL_Delay(ms);
}
