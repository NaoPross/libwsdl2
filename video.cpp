#include "wsdl2/video.hpp"
#include "wsdl2/debug.hpp"

#include <exception>

extern "C" {
#include <SDL2/SDL.h>

#ifdef WSDL2_IMG
#include <SDL2/SDL_image.h>
#endif
}

using namespace wsdl2;

/* class surface */

surface::surface(surface&& other) {
    npdebug("moved surface");
    m_surface = other.m_surface;
    other.m_surface = nullptr;
}

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

    npdebug("created surface");
}

surface::surface(void *pixels, std::size_t width, std::size_t height, int depth, int pitch,
    int rmask /* = 0 */, int gmask /* = 0 */, int bmask /* = 0 */, int amask /* = 0 */
) {
    m_surface = SDL_CreateRGBSurfaceFrom(pixels,
        static_cast<int>(width), static_cast<int>(height),
        depth, pitch, rmask, gmask, bmask, amask
    );

    if (m_surface == NULL) {
        throw std::runtime_error("failed to create SDL_Surface from pixels");
    }

    npdebug("crated surface from pixels");
}

// private constructor
surface::surface(SDL_Surface* surf)
{
    if (surf == nullptr) {
        throw std::runtime_error("failed to create SDL_Surface");
    }

    m_surface = surf;
    npdebug("created surface from ptr");
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

#ifdef WSDL2_IMG
        IMG_Load(path.c_str()); // use the image library
#else
        SDL_LoadBMP(path.c_str()); // load simply a bitmap
#endif

    if (surf == NULL) 
        return std::nullopt;

    return surface(surf);
}



/* class Renderer */

renderer::renderer() {
    npdebug("warning: created uninitialized renderer object");
}

renderer::renderer(renderer&& other) {
    m_renderer = std::move(other.m_renderer);
}

bool renderer::set_target(texture& target) {
    if (target.pixel_access() != texture::access::target)
        return false;

    util::check(0 == SDL_SetRenderTarget(sdl(), target.sdl()));
    return true;
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

renderer::renderer(SDL_Window *win)  {
    // create a rendering contest
    m_renderer = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!m_renderer) {
        throw std::runtime_error("failed to create SDL renderer");
    }
}

renderer::renderer(window& w) {
    renderer(w.sdl());
    npdebug("created renderer from window");
}

renderer::~renderer() {
    if (m_renderer != NULL) {
        SDL_DestroyRenderer(m_renderer);
        npdebug("destroyed renderer");
    }
}

wsdl2::point renderer::size() const
{
    point out;
    SDL_GetRendererOutputSize(const_cast<SDL_Renderer*>(m_renderer), &out.x, &out.y);

    return out;
}

/* class texture */

texture::texture(renderer& r, texture::access a, int width, int height, pixelformat::format p)
    : m_renderer(r), m_width(width), m_height(height), m_format(p)
{
    m_texture = SDL_CreateTexture(r.m_renderer, 
        static_cast<Uint32>(m_format), static_cast<int>(a), 
        static_cast<int>(m_width), static_cast<int>(m_height)
    );
}

texture::texture(texture&& other)
    : m_renderer(other.m_renderer), 
      m_width(other.m_width), 
      m_height(other.m_height),
      m_format(other.m_format),
      m_texture(other.m_texture)
{
    other.m_texture = nullptr;
}

texture::texture(renderer& r, surface& surf, pixelformat::format p)
    : m_renderer(r), m_width(surf.width()), m_height(surf.height()), m_format(p)
{
    if (p != pixelformat::format::unknown)
    {
        // TODO, create a new surface with the right pixelformat
    }

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
    // access is always static
    // m_access = static_cast<access>(acc);
    //m_width = static_cast<std::size_t>(w);
    //m_height = static_cast<std::size_t>(h);
}

texture::~texture() {
    if (m_texture != NULL) {
        SDL_DestroyTexture(m_texture);
        npdebug("destroyed texture");
    }
}

std::shared_ptr<texture> static_texture::load(const std::string& path, renderer& r)
{
    npdebug("Loading ", path, " as surface")
    auto surf = surface::load(path);
    
    npdebug(path, " loaded successfully")

    if (surf) 
        return std::static_pointer_cast<texture>(std::make_shared<static_texture>(r, *surf));
    else { 
#ifdef IMG_LOADING
        npdebug("Unable to load image ", path, "! SDL_image Error: ", IMG_GetError()); 
#else
        npdebug("Unable to load image ", path); 
#endif
    } 
    
    return nullptr; // nullopt
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

streaming_texture::streaming_texture(renderer& r, surface& surf, pixelformat::format p)
    : texture(r, access::streaming, surf.width(), surf.height(), p)
{
    // blit the surface
    surface buf = lock();
    surf.blit(buf); // write onto the buffer
    unlock(); // save changes
}

/* class window */

// code used by events
std::unordered_map<unsigned, window*> window::_windows;

window::window(window&& other)
    : m_open(other.m_open),
      m_window(other.m_window),
      m_id(other.m_id),
      m_renderer(std::move(other.m_renderer))
{
    other.m_window = NULL;
}

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
      m_id(SDL_GetWindowID(m_window)),
      m_renderer(new renderer(m_window))
{
    // put into window id mapping
    _windows.insert({m_id, this});

    npdebug("creaded window");
}

window::~window() {
    // remove from window id mapping
    _windows.erase(m_id);

    // destroy renderer
    m_renderer.reset();

    // destroy window
    if (m_window != NULL) {
        SDL_DestroyWindow(m_window);
        npdebug("destroyed window");
    }
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

wsdl2::point window::size() const
{
    wsdl2::point out;
    SDL_GL_GetDrawableSize(const_cast<SDL_Window*>(m_window), &out.x, &out.y);

    return out;
}
