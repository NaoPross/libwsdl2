#include "../debug.hpp"

extern "C" {
#include <SDL2/SDL_error.h>
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
#ifdef DEBUG
            if (!expr) {
                npdebug("an internal SDL error occurred:");
                npdebug("  ", const_cast<const char * const>(SDL_GetError()));
            }
#endif
            return expr;
        }
    }
}
