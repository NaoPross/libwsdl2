#pragma once
#ifndef WSDL2_TTF
#warning "libwrapsdl2 is complied without support for SDL2_ttf"
#else

extern "C" {
#include <SDL2/SDL2_ttf.h>
}

namespace wsdl2 {
    namespace ttf {
    }
}

#endif
