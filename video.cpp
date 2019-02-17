#include "video.hpp"
#include "debug.hpp"

#include <exception>

extern "C" {
#include <SDL2/SDL.h>

#ifdef IMG_LOADING
#include <SDL2/SDL_Image.h>
#endif
}

using namespace wsdl2;

/* class surface */

surface::surface(std::size_t width, std::size_t height, int depth /* = 24 */,
    int rmask /* = 0 */, int gmask /* = 0 */, int bmask /* = 0 */, int amask /* = 0 */
) {
    m_surface = SDL_CreateRGBSurface(0,
        static_cast<int>(width),
        static_cast<int>(height),
        depth, rmask, gmask, bmask, amask
    );

    if (m_surface == NULL) {
        throw std::runtime_error("failed to create SDL_Surface");
    }
}

surface::surface(surface&& other) {
    npdebug("moved surface");
    m_surface = other.m_surface;
    other.m_surface = nullptr;
}

// private constructor
surface::surface(SDL_Surface* surf)
{
    if (surf == NULL) {
        throw std::runtime_error("failed to create SDL_Surface");
    }

    m_surface = surf;
}

surface::~surface() {
    npdebug("deleted surface");
    if (m_surface != NULL) {
        npdebug("freed surface");
        SDL_FreeSurface(m_surface);
    }
}


SDL_Surface* surface::sdl() {
#ifdef DEBUG
    if (m_surface == NULL) {
        throw std::runtime_error(
            "attempted to call texture::sdl() when m_texture is NULL"
        );
    }
#endif

    return m_surface;
}

SDL_Surface* surface::sdl() const {
#ifdef DEBUG
    if (m_surface == NULL) {
        throw std::runtime_error(
            "attempted to call texture::sdl() when m_texture is NULL"
        );
    }
#endif

    return m_surface;
}

std::optional<surface> surface::load(const std::string& path)
{
    SDL_Surface* surf = 

#ifdef IMG_LOADING
        IMG_Load(path.c_str()); // use the image library
#else
        SDL_LoadBMP(path.c_str()); // load simply a bitmap
#endif

    if (surf == NULL) 
        return std::nullopt;

    return surface(surf);
}



/* class renderer */

renderer::renderer() {
    npdebug("warning: created uninitialized renderer object");
}

void renderer::set_target(texture& target) {
    util::check(0 == SDL_SetRenderTarget(sdl(), target.sdl()));
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
}



/* class texture */

texture::texture(renderer& r, pixelformat::format p, texture::access a,
                 std::size_t width, std::size_t height)

    : m_renderer(r), m_format(p), m_access(a), m_width(width), m_height(height)
{
    m_texture = SDL_CreateTexture(r.m_renderer, 
        static_cast<Uint32>(m_format), static_cast<int>(m_access), 
        static_cast<int>(m_width), static_cast<int>(m_height)
    );
}

texture::texture(texture&& other)
    : m_renderer(other.m_renderer), m_texture(other.m_texture),
      m_format(other.m_format), m_access(other.m_access),
      m_width(other.m_width), m_height(other.m_height)
{
    other.m_texture = nullptr;
}

texture::texture(renderer& r, surface& surf)
    : m_renderer(r)
{
    m_texture = SDL_CreateTextureFromSurface(r.sdl(), surf.sdl()); 
    
    util::check(m_texture != NULL);
    if (m_texture == NULL) {
        throw std::runtime_error("Unable to create texture from surface"); 
    }

    // obtain informations
    int acc, w, h;
    Uint32 form;
    SDL_QueryTexture(m_texture, &form, &acc, &w, &h);

    m_format = static_cast<pixelformat::format>(form);
    m_access = static_cast<access>(acc);
    m_width = static_cast<std::size_t>(w);
    m_height = static_cast<std::size_t>(h);
}

texture::~texture() {
    if (m_texture != NULL)
        SDL_DestroyTexture(m_texture);
}

std::optional<texture> texture::load(const std::string& path, renderer& r)
{
    npdebug("Loading ", path, " as surface")
    auto surf = surface::load(path);
    
    npdebug(path, " loaded successfully")

    if (surf) 
        return texture(r, *surf);
    else { 
#ifdef IMG_LOADING
        npdebug("Unable to load image ", path, "! SDL_image Error: ", IMG_GetError()); 
#else
        npdebug("Unable to load image ", path); 
#endif
    } 
    
    return std::nullopt; // nullopt
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


/* class window */

// code used by events
std::unordered_map<unsigned, window*> window::_windows;

window::window(const std::string& title, std::size_t width, std::size_t height)
    : m_open(false),
      // create (hidden) window
      m_window(SDL_CreateWindow(
          title.c_str(),
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          static_cast<int>(width),
          static_cast<int>(height),
          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
      )),
      m_id(SDL_GetWindowID(m_window))
{
    // put into window id mapping
    _windows.insert({m_id, this});

    // create renderer
    m_renderer.create_sdl_renderer(m_window);
}

window::~window() {
    // remove from window id mapping
    _windows.erase(m_id);

    // destroy window
    if (m_window != NULL)
        SDL_DestroyWindow(m_window);
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

/* static members for class window */
window& window::get(unsigned id)
{
    return *window::_windows[id];
}

