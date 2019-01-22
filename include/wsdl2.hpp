#pragma once

extern "C" {
#include <SDL2/SDL.h>
}

namespace wsdl2 {
    bool initialize(void);
    void quit(void);

    namespace util {
        constexpr inline bool sdl_bool(SDL_bool b) {
            return b == SDL_TRUE;
        }

        constexpr inline SDL_bool sdl_bool(bool b) {
            return (b) ? SDL_TRUE : SDL_FALSE;
        }
    }
    
    // tool functions
    void delay(unsigned ms);
}