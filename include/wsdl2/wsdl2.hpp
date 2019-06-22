#pragma once

extern "C" {
#include <SDL2/SDL.h>
}

namespace wsdl2 {

    bool initialize(void);

    void quit(void);
    
    // tool functions
    void delay(unsigned ms);
}
