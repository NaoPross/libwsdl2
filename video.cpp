#include "video.hpp"
#include "debug.hpp"

#include <exception>
#include <cstdint>

extern "C" {
#include <SDL2/SDL.h>
}

using namespace wrapsdl2;

/* class window */

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

    // create a rendering contest
    m_renderer = SDL_CreateRenderer(
        m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // other attributes
    m_open = false;
}

window::~window() {
    if (m_renderer != NULL)
        SDL_DestroyRenderer(m_renderer);
    else
        npdebug("warning: m_renderer is NULL")

    if (m_window != NULL)
        SDL_DestroyWindow(m_window);
    else
        npdebug("warning: m_window is NULL")
}


void window::open() {
    show();
    m_open = true;
}

void window::close() {
    hide();
    m_open = false;
}

bool window::is_open() {
    return m_open;
}

bool window::is_visible() {
    return SDL_WINDOW_SHOWN & SDL_GetWindowFlags(m_window);
}

void window::update() {
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
}