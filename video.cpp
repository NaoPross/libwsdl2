#include "video.hpp"
#include "debug.hpp"

#include <exception>
#include <cstdint>

extern "C" {
#include <SDL2/SDL.h>
}

using namespace wrapsdl2;

window::window(const std::string& title, std::size_t width, std::size_t height) {
    // create (hidden) window
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(width),
        static_cast<int>(height),
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
    );

    if (m_window == NULL) {
        throw std::runtime_error("failed to create SDL window");
    }

    // create surface
    m_surface = SDL_GetWindowSurface(m_window);
}

window::~window() {
    SDL_FreeSurface(m_surface);
    SDL_DestroyWindow(m_window);
}


void window::show() { SDL_ShowWindow(m_window); }
void window::hide() { SDL_HideWindow(m_window); }
void window::raise() { SDL_RaiseWindow(m_window); }

bool window::visible() {
    std::uint32_t flags = SDL_GetWindowFlags(m_window);

    return flags & SDL_WINDOW_SHOWN;
}

void window::update() {
    SDL_UpdateWindowSurface(m_window);
}