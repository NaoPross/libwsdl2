#include "wrapsdl.hpp"
#include "debug.hpp"

extern "C" {
#include <SDL2/SDL.h>
}

bool wrapsdl::initialize(void) {
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		npdebug("failed to initialize sdl video subsystem");
		return false;
	}

	return true;
}

void wrapsdl::quit(void) {
	SDL_Quit();
}
