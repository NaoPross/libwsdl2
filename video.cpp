#include "video.hpp"
#include "debug.hpp"

#include <exception>
#include <cstdint>

#include <vector>

extern "C" {
#include <SDL2/SDL.h>
}

using namespace wsdl2;


/* class texture */

texture::texture(renderer& r, texture::pixformat p, texture::access a,
                 std::size_t width, std::size_t height)

    : _format(p), _access(a), m_renderer(r)
{
    m_texture = SDL_CreateTexture(r.m_renderer, 
        static_cast<Uint32>(p), static_cast<int>(a), 
        static_cast<int>(width), static_cast<int>(height)
    );
}

texture::~texture() {
    if (m_texture != NULL)
        SDL_DestroyTexture(m_texture);
    else
        npdebug("warning: m_texture is NULL");
}

SDL_Texture* texture::sdl() {
#ifdef DEBUG
    if (m_texture == NULL) {
        throw std::runtime_error(
            "attempted to call texture::sdl() when m_texture is NULL"
        );
    }
#endif

    return m_texture;
}

/* class renderer */

renderer::renderer() {
    npdebug("warning: created uninitialized renderer object");
}

SDL_Renderer * renderer::sdl() {
#ifdef DEBUG
    if (m_renderer == NULL) {
        throw std::runtime_error(
            "attempted to call renderer::sdl() when m_renderer is NULL"
        );
    }
#endif

    return m_renderer;
}

void renderer::create_sdl_renderer(SDL_Window *win)  {
    // create a rendering contest
    m_renderer = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (m_renderer == NULL) {
        throw std::runtime_error("failed to create SDL renderer");
    }
}

renderer::renderer(window& w) {
    create_sdl_renderer(w.sdl());
}

renderer::~renderer() {
    if (m_renderer != NULL)
        SDL_DestroyRenderer(m_renderer);
    else
        npdebug("warning: m_renderer is NULL")
}

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

    m_renderer.create_sdl_renderer(m_window);

    // other attributes
    m_open = false;
}

window::~window() {
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
    m_renderer.clear();
    m_renderer.present();
}

SDL_Window * window::sdl() {
#ifdef DEBUG
    if (m_window == NULL) {
        throw std::runtime_error(
            "attempted to call window::sdl() when m_window is NULL"
        );
    }
#endif

    return m_window;
}
