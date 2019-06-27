#pragma once
#include "debug.hpp"

extern "C" {
#include <SDL2/SDL_error.h>

#ifdef WSDL2_TTF
#include <SDL2/SDL_ttf.h>
#endif
}

namespace wsdl2 {
    namespace util {
        // constexpr inline bool sdl_bool(SDL_bool b) {
        //     return b == SDL_TRUE;
        // }

        // constexpr inline SDL_bool sdl_bool(bool b) {
        //     return (b) ? SDL_TRUE : SDL_FALSE;
        // }

        constexpr inline bool check(bool expr) {
#ifndef NDEBUG
            if (!expr) {
                npdebug("an internal SDL error occurred:");
                npdebug("  ", const_cast<const char * const>(SDL_GetError()));
            }
#endif
            return expr;
        }

#ifdef WSDL2_TTF
        constexpr inline bool check_ttf(bool expr) {
#ifndef NDEBUG
            if (!expr) {
                npdebug("an internal SDL_ttf error occurred:");
                npdebug("  ", const_cast<const char * const>(TTF_GetError()));
            }
#endif
            return expr;
        }
#endif
    }
}
