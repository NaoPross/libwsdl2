#pragma once

#include "util.hpp"

#include <string>
#include <array>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>

extern "C" {
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
}

#ifdef WSDL2_USE_MM
#include <mm/mmvec.hpp>
#endif

namespace wsdl2 {
    // forward declarations
    class texture;
    class renderer;
    class window;

    namespace event {
        class event;
    }

    // name aliases

    /* equivalent to
     * struct point {
     *     int x, y;
     * };
     */
    using point = SDL_Point;

    /* extension of SDL_Rect using black magic
     * equivalent to
     *
     * struct rect {
     *     int x, y, w, h;
     *     ... methods
     * };
     *
     * SDL_Rect can be cast to rect and vice versa
     */
    struct rect : public SDL_Rect {
        rect()            = default;
        rect(rect&)       = default;
        rect(const rect&) = default;

        // emulate aggregate initialization with magic
        template<typename ...Args,
                 std::enable_if_t<
                     std::conjunction<
                         std::is_same<int, typename std::decay<Args>::type>...
                         >::value>>
        rect(Args&& ...l) {
            static_assert(sizeof ...(l) == 4, "rect has requires x, y, w, h");
            // use contructor rect(x, y, w, h)
            rect(std::forward<Args>(l)...);
        }

        rect(int _x, int _y, int _w, int _h) : SDL_Rect({_x, _y, _w, _h}) {}
        rect(SDL_Rect& r) : SDL_Rect(r) {}
        rect(SDL_Rect&& r) : SDL_Rect(r) {}

        bool operator==(const rect& other) const {
            return ((x == other.x)
                    && (y == other.y)
                    && (w == other.w)
                    && (h == other.h));
        }

        inline bool intersects(const rect& other) const {
            if (SDL_TRUE == SDL_HasIntersection(this, &other))
                return true;

            return false;
        }

        inline std::optional<rect> intersection(const rect& other) const {
            rect result;
            if (SDL_TRUE == SDL_IntersectRect(this, &other, &result))
                return result;

            return std::nullopt;
        }

        inline bool contains(const point& p) const {
            return (SDL_TRUE == SDL_PointInRect(&p, this));
        }

        inline bool empty() const {
            return (h <= 0 && w <= 0);
        }

        /// return a rectangle that is the union of two rectangles
        static rect union_(const rect& a, const rect& b) {
            rect res;
            SDL_UnionRect(&a, &b, &res);
            return res;
        }

        rect union_with(const rect& other) {
            return rect::union_(*this, other);
        }

#ifdef WSDL2_USE_MM
        /// convert to 4d vector
        operator mm::vec<int, 4>() {
            return { x, y, w, h };
        }

        mm::vec2<int> pos() {
            return { x, y };
        }

        void pos(mm::vec2<int> newpos) {
            x = newpos.x();
            y = newpos.y();
        }

        void move(mm::vec2<int> v) {
            x += v.x();
            y += v.y();
        }

        mm::vec2<int> size() {
            return { w, h };
        }

        void resize(mm::vec2<int> newsize) {
            w = newsize.x();
            h = newsize.y();
        }
#endif
    };

    /* equivalent to 
     * struct color {
     *     std::uint8_t r, g, b, a;
     * };
     */
    using color = SDL_Color;

    // TODO: implement blend in texture
    enum class blend_mode {
        none  = SDL_BLENDMODE_NONE,
        blend = SDL_BLENDMODE_BLEND,
        add   = SDL_BLENDMODE_ADD,
        mod   = SDL_BLENDMODE_MOD,
    };

    /// stores informations about the pixel format
    class pixelformat {
    public:
        enum class format: Uint32 {
            unknown     = SDL_PIXELFORMAT_UNKNOWN,
            index1lsb   = SDL_PIXELFORMAT_INDEX1LSB,
            index1msb   = SDL_PIXELFORMAT_INDEX1MSB,
            index4lsb   = SDL_PIXELFORMAT_INDEX4LSB,
            index4msb   = SDL_PIXELFORMAT_INDEX4MSB,
            index8      = SDL_PIXELFORMAT_INDEX8,
            rgb332      = SDL_PIXELFORMAT_RGB332,
            rgb444      = SDL_PIXELFORMAT_RGB444,
            rgb555      = SDL_PIXELFORMAT_RGB555,
            bgr555      = SDL_PIXELFORMAT_BGR555,
            argb4444    = SDL_PIXELFORMAT_ARGB4444,
            rgba4444    = SDL_PIXELFORMAT_RGBA4444,
            abgr4444    = SDL_PIXELFORMAT_ABGR4444,
            bgra4444    = SDL_PIXELFORMAT_BGRA4444,
            argb1555    = SDL_PIXELFORMAT_ARGB1555,
            rgba5551    = SDL_PIXELFORMAT_RGBA5551,
            abgr1555    = SDL_PIXELFORMAT_ABGR1555,
            bgra5551    = SDL_PIXELFORMAT_BGRA5551,
            rgb565      = SDL_PIXELFORMAT_RGB565,
            bgr565      = SDL_PIXELFORMAT_BGR565,
            rgb24       = SDL_PIXELFORMAT_RGB24,
            bgr24       = SDL_PIXELFORMAT_BGR24,
            rgb888      = SDL_PIXELFORMAT_RGB888,
            rgbx8888    = SDL_PIXELFORMAT_RGBX8888,
            bgr888      = SDL_PIXELFORMAT_BGR888,
            bgrx8888    = SDL_PIXELFORMAT_BGRX8888,
            argb8888    = SDL_PIXELFORMAT_ARGB8888,
            rgba8888    = SDL_PIXELFORMAT_RGBA8888,
            abgr8888    = SDL_PIXELFORMAT_ABGR8888,
            bgra8888    = SDL_PIXELFORMAT_BGRA8888,
            argb2101010 = SDL_PIXELFORMAT_ARGB2101010,
        };

        pixelformat(SDL_PixelFormat *p);

        // TODO: access members

    private:
        SDL_PixelFormat *m_pixelformat;
    };


#ifdef WSDL2_TTF
    namespace ttf {
        class font;
    }
#endif

    /// a graphical object allocated in the RAM
    class surface {
    public:

        friend class texture;
#ifdef WSDL2_TTF
        friend class ttf::font;
#endif

        surface() = delete;
        virtual ~surface();
        
        surface(surface&& other);
        surface(std::size_t width, std::size_t height, int depth = 24,
                int rmask = 0, int gmask = 0, int bmask = 0, int amask = 0);

        // constructor from raw pixels 
        surface(void *pixels, std::size_t width, std::size_t height, int depth, int pitch,
                int rmask = 0, int gmask = 0, int bmask = 0, int amask = 0);

        // TODO: copy with SDL_BlitSurface(src, srect, dst, drect)
        // surface(const surface& other);

        /// make a surface from an image
        // static surface from_bmp(path);
        // static surface from_png(path);
        // static surface from_jpg(path)

        /// access members
        inline int width() { return sdl()->w; }
        inline int height() { return sdl()->h; }

        inline rect clip() { return static_cast<rect>(sdl()->clip_rect); }
        inline bool clip(const rect& r) {
            return (SDL_TRUE == SDL_SetClipRect(sdl(), &r));
        }

        /// copy a surface into another
        inline static void blit(surface& src, surface& dest) {
            // copies the entire surface src, into dest at (0,0)
            util::check(0 == SDL_BlitSurface(src.sdl(), NULL, dest.sdl(), NULL));
        }

        inline static void blit(surface& src, rect& src_r, surface& dest, rect& dest_r) {
            util::check(0 == SDL_BlitSurface(src.sdl(), &src_r, dest.sdl(), &dest_r));
        }

        inline static void blit_scaled(surface& src, surface& dest) {
            // copies the entire surface src, into dest at (0,0)
            util::check(0 == SDL_BlitScaled(src.sdl(), NULL, dest.sdl(), NULL));
        }

        inline static void blit_scaled(surface& src, rect& src_r, surface& dest, rect& dest_r) {
            util::check(0 == SDL_BlitScaled(src.sdl(), &src_r, dest.sdl(), &dest_r));
        }

        inline void blit(surface& dest) {
            blit(*this, dest);
        }

        inline void blit_scaled(surface& dest) {
            blit_scaled(*this, dest);
        }

        /// fill a rectangle
        inline void fill_rect(const rect& r, const color& c) {
            util::check(0 == SDL_FillRect(sdl(), &r, 
                SDL_MapRGBA(sdl()->format, c.r, c.g, c.b, c.a)
            ));
        }

        inline void fill_rect(const rect& re, uint8_t r, uint8_t g, uint8_t b) {
            util::check(0 == SDL_FillRect(sdl(), &re, 
                SDL_MapRGB(sdl()->format, r, g, b)
            ));
        }

        /// fill many rectangles
        template<template<typename> typename Container>
        inline void fill_rects(const Container<rect>& rects, const color& c) {
            util::check(0 == SDL_RenderFillRects(sdl(), rects.data(), rects.size(),
                SDL_MapRGBA(sdl()->format, c.r, c.g, c.b, c.a)
            ));
        }

        template<template<typename> typename Container>
        inline void fill_rects(const Container<rect>& rects, uint8_t r, uint8_t g, uint8_t b) {
            util::check(0 == SDL_RenderFillRects(sdl(), rects.data(), rects.size(),
                SDL_MapRGB(sdl()->format, r, g, b)
            ));
        }

        /// fill the entire surface
        inline void fill(const color& c) {
            util::check(0 == SDL_FillRect(sdl(), NULL, 
                SDL_MapRGBA(sdl()->format, c.r, c.g, c.b, c.a)
            ));
        }

        inline void fill(uint8_t r, uint8_t g, uint8_t b) {
            util::check(0 == SDL_FillRect(sdl(), NULL, 
                SDL_MapRGB(sdl()->format, r, g, b)
            ));
        }

        /// memory locking and unlocking for pixels()
        inline void unlock() { SDL_UnlockSurface(sdl()); }
        inline void lock() {
            util::check(0 == SDL_LockSurface(sdl()));
        }

        inline bool mustlock() {
            return (SDL_MUSTLOCK(m_surface) == SDL_TRUE);
        }

        /// set alpha
        inline bool alpha(std::uint8_t val) {
            int supported = SDL_SetSurfaceAlphaMod(sdl(), val);
            if (supported == -1)
                return false;
            else if (supported == 0)
                return true;

            util::check(supported >= -1);
            return false;

        }

        inline std::uint8_t alpha() {
            std::uint8_t val;
            util::check(0 == SDL_GetSurfaceAlphaMod(sdl(), &val));

            return val;
        }

        static std::optional<surface> load(const std::string& path);
        
        // how about we don't allow this
        // void * pixels()

        /// to enable RLE optimization
        inline void use_rle(bool enable) {
            util::check(0 == SDL_SetSurfaceRLE(sdl(), enable));
        }

        // TODO: finish pixelformat
        // SDL_Surface convert(pixelformat f);

    private:

        // take the ownership of the sdl surface
        // used for loading
        surface(SDL_Surface*);

        SDL_Surface *m_surface;

        // dirty C code
        SDL_Surface* sdl();
        SDL_Surface* sdl() const;
    };


    /// the guy who does the actual hard stuff
    class renderer {
    public:
        friend class window;
        friend class texture;

        enum class flip {
            none       = SDL_FLIP_NONE,
            horizontal = SDL_FLIP_HORIZONTAL,
            vertical   = SDL_FLIP_VERTICAL,
        };

        renderer(window& w);
        renderer(texture& t);

        renderer(renderer&& other);
        virtual ~renderer();

        // TODO: check that is a target texture
        bool set_target(texture& target);
        inline void clear() { util::check(0 == SDL_RenderClear(sdl())); }
        inline void present() { SDL_RenderPresent(sdl()); }

        // viewport
        inline void reset_viewport() {
            util::check(0 == SDL_RenderSetViewport(sdl(), NULL));
        }

        inline void viewport(const rect& v) {
            util::check(0 == SDL_RenderSetViewport(sdl(), &v));
        }

        inline rect viewport() {
            rect r;
            SDL_RenderGetViewport(sdl(), &r);

            return r;
        }

        // set color

        inline void set_color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
            util::check(0 == SDL_SetRenderDrawColor(sdl(), r, g, b, a));
        }

        inline void set_color(const color& c) {
            set_color(c.r, c.g, c.b, c.a);
        }

        inline color get_color() {
            color c;
            util::check(0 == SDL_GetRenderDrawColor(sdl(), &c.r, &c.g, &c.b, &c.a));
            return c;
        }

        // draw a single element

        inline void draw_point(int x, int y) {
            util::check(0 == SDL_RenderDrawPoint(sdl(), x, y));
        }

        inline void draw_point(const point& p) {
            draw_point(p.x, p.y);
        }

        inline void draw_line(int x1, int y1, int x2, int y2) {
            util::check(0 == SDL_RenderDrawLine(sdl(), x1, y1, x2, y2));
        };

        inline void draw_line(const point& a, const point& b) {
            draw_line(a.x, a.y, b.x, b.y);
        };

        inline void draw_rect(int x, int y, int h, int w) {
            draw_rect(rect {x, y, h, w});
        }

        inline void draw_rect(const rect& r) {
            util::check(0 == SDL_RenderDrawRect(sdl(), &r));
        }

        inline void fill_rect(const rect& r) {
            util::check(0 == SDL_RenderFillRect(sdl(), &r));
        }

        // draw multiple elements

        template<template<typename> typename Container>
        inline void draw_lines(const Container<point>& points) {
            util::check(0 == SDL_RenderDrawLines(sdl(), points.data(), points.size()));
        }

        template<template<typename> typename Container>
        inline void draw_points(const Container<point>& points) {
            util::check(0 == SDL_RenderDrawLines(sdl(), points.data(), points.size()));
        }

        template<template<typename> typename Container>
        inline void draw_rects(const Container<rect>& rects) {
            util::check(0 == SDL_RenderDrawLines(sdl(), rects.data(), rects.size()));
        }

        template<template<typename> typename Container>
        inline void fill_rects(const Container<rect>& rects) {
            util::check(0 == SDL_RenderFillRects(sdl(), rects.data(), rects.size()));
        }

        // overloaded drawing function

        inline void draw(point&& p) { draw_point(std::forward<point>(p)); }
        inline void draw(rect&& r)  { draw_rect(std::forward<rect>(r)); }
        inline void fill(rect&& r)  { fill_rect(std::forward<rect>(r)); }

        template<template<typename> typename Container>
        inline void draw(Container<point>&& points) {
            draw_points(std::forward<Container<point>>(points));
        }

        template<template<typename> typename Container>
        inline void draw(Container<rect>&& rects) {
            draw_rects(std::forward<Container<rect>>(rects));
        }

        template<template<typename> typename Container>
        inline void fill(const Container<rect>& rects) {
            fill_rects(rects);
        }

        // fill the entire texture
        inline void fill() {
            util::check(0 == SDL_RenderFillRect(sdl(), NULL));
        }


    private:
        SDL_Renderer *m_renderer = NULL;

        renderer();
        renderer(SDL_Window *win);

        // dirty C code
        SDL_Renderer* sdl();
    };

    struct static_texture;
    struct streaming_texture;
    struct target_texture;

    /// a graphic object allocated in the VRAM,
    class texture {
    public:
        friend class renderer;

        enum class access : int {
            static_ = SDL_TEXTUREACCESS_STATIC,
            streaming = SDL_TEXTUREACCESS_STREAMING,
            target = SDL_TEXTUREACCESS_TARGET
        };


        texture() = delete;
        texture(const texture& other) = delete;

        texture(texture&& other);
        texture(renderer&, surface& surf);

        texture(renderer& r, pixelformat::format p, access a,
                int width, int height);

        virtual ~texture();

        inline void render() {
            util::check(0 == SDL_RenderCopy(
                m_renderer.sdl(), m_texture, NULL, NULL
            ));
        }

        inline void render(rect& src, rect& dest) {
            util::check(0 == SDL_RenderCopy(
                m_renderer.sdl(), m_texture, &src, &dest
            ));
        }

        inline void render(rect& src, rect& dest, 
            const double angle, const point& center, renderer::flip flip)
        {
            util::check(0 == SDL_RenderCopyEx(
                m_renderer.sdl(), m_texture,
                &src, &dest, angle, &center,
                static_cast<SDL_RendererFlip>(flip)
            ));
        }

        /// lock to be write-only
        inline surface lock() {
            void *pixels;
            int pitch;

            util::check(0 == SDL_LockTexture(
                m_texture, NULL, &pixels, &pitch
            ));

            return surface(pixels, width(), height(), 24, pitch);
        }

        /// lock a portion to be write-only
        inline surface lock(const rect& region) {
            void *pixels;
            int pitch;

            util::check(0 == SDL_LockTexture(
                m_texture, &region, &pixels, &pitch
            ));

            // TODO: un-hardcode 24 bit depth
            return surface(pixels, region.w, region.h, 24, pitch);
        }

        inline void unlock() { SDL_UnlockTexture(m_texture); }

        inline bool alpha(std::uint8_t val) {
            int supported = SDL_SetTextureAlphaMod(m_texture, val);
            if (supported == -1)
                return false;
            else if (supported == 0)
                return true;

            util::check(supported >= -1);
            return false;

        }

        inline std::uint8_t alpha() const {
            std::uint8_t val;
            util::check(0 == SDL_GetTextureAlphaMod(m_texture, &val));

            return val;
        }
        
        inline access pixel_access() const {
            return m_access;
        }

        inline pixelformat::format pixel_format() const {
            return m_format;
        }

        inline int width() const {
            return m_width;
        }

        inline int height() const {
            return m_height;
        }

        static std::optional<texture> load(const std::string& path, renderer&);

        // operator static_texture() {
        // }

    private:
        renderer& m_renderer;
        SDL_Texture *m_texture;

        pixelformat::format m_format;
        const access m_access;
        int m_width;
        int m_height;

        // dirty C code
        SDL_Texture* sdl();
    };

    struct static_texture : private texture
    {
        using texture::texture;

        static_texture(renderer& r, pixelformat::format p, int width, int height)
            : texture(r, p, texture::access::static_, width, height) {}

        using texture::render;

        using texture::alpha;

        using texture::pixel_access;
        using texture::pixel_format;

        using texture::width;
        using texture::height;
    };

    struct streaming_texture : private texture
    {
        using texture::texture;

        streaming_texture(renderer& r, pixelformat::format p, int width, int height)
            : texture(r, p, texture::access::streaming, width, height) {}
        
        using texture::render;

        using texture::lock;
        using texture::lock;
        using texture::unlock;

        using texture::alpha;

        using texture::pixel_access;
        using texture::pixel_format;

        using texture::width;
        using texture::height;
    };

    struct target_texture : private texture
    {
        using texture::texture;

        target_texture(renderer& r, pixelformat::format p, int width, int height)
            : texture(r, p, texture::access::target, width, height) {}

        using texture::render;

        using texture::alpha;

        using texture::pixel_access;
        using texture::pixel_format;

        using texture::width;
        using texture::height;
    };

    /// a basic wrapper around a SDL window
    class window {
    public:
        friend class renderer;
        friend class event::event;

        window() = delete;
        window(const window& other) = delete;
        window(window&& other);

        window(const std::string& title, std::size_t width, std::size_t height);
        virtual ~window();

        // setters
        void open();
        void close();

        inline void show() { SDL_ShowWindow(sdl()); }
        inline void hide() { SDL_HideWindow(sdl()); }
        inline void raise() { SDL_RaiseWindow(sdl()); }

        // getters
        bool is_open();
        bool is_visible();

        // rendering
        renderer& get_renderer() const { return *m_renderer; }
        void clear() const { m_renderer->clear(); }
        void present() const { m_renderer->present(); }
            

        static window& get(unsigned id);

    private:
        bool m_open;
        SDL_Window *m_window = NULL;
        const unsigned m_id;

        mutable std::unique_ptr<renderer> m_renderer;

        // dirty C code
        SDL_Window* sdl();

        // code used by events
        static std::unordered_map<unsigned, window*> _windows;
    };
}
