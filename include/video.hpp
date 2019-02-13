#pragma once

#include "util.hpp"

#include <string>
#include <array>
#include <type_traits>
#include <optional>
#include <unordered_map>

extern "C" {
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
}

namespace wsdl2 {
    // forward declarations
    class texture;
    class renderer;
    class window;
    namespace event {
        class event;
    }

    // name aliases
    using rect = SDL_Rect;
    /* equivalent to
     * struct rect {
     *     int x, y, w, h;
     * };
     */

    using point = SDL_Point;
    /* equivalent to
     * struct point {
     *     int x, y;
     * };
     */

    using color = SDL_Color;
    /* equivalent to 
     * struct color {
     *     std::uint8_t r, g, b, a;
     * };
     */

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

    /// a graphical object allocated in the RAM
    class surface {
    public:
        surface() = delete;
        virtual ~surface();
        
        surface(std::size_t width, std::size_t height, int depth = 24,
                int rmask = 0, int gmask = 0, int bmask = 0, int amask = 0);

        // TODO: constructor from raw pixels 
        //       with SDL_CreateSurfaceFrom or SDL_CreateGBSurfaceWithFormatFrom or
        //       with SDL_CreateRGBSurfaceFrom or SDL_CreateRGBSurfaceWithFormatFrom
        // surface(void *pixels, ...)

        // TODO: copy with SDL_BlitSurface(src, srect, dst, drect)
        // surface(const surface& other);

        /// make a surface from an image
        // static surface from_bmp(path);
        // static surface from_png(path);
        // static surface from_jpg(path)

        /// access members
        inline int width() { return sdl()->w; }
        inline int height() { return sdl()->h; }

        inline rect clip() { return sdl()->clip_rect; }
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

        
        // how about we don't allow this
        // void * pixels()

        /// to enable RLE optimization
        inline void use_rle(bool enable) {
            util::check(0 == SDL_SetSurfaceRLE(sdl(), enable));
        }

        // TODO: finish pixelformat
        // SDL_Surface convert(pixelformat f);

    private:
        SDL_Surface *m_surface;

        // dirty C code
        SDL_Surface* sdl();
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
        virtual ~renderer();

        void set_target(texture& target);
        inline void clear() { SDL_RenderClear(sdl()); }
        inline void present() { SDL_RenderPresent(sdl()); }

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
        SDL_Renderer *m_renderer;

        renderer();
        void create_sdl_renderer(SDL_Window *win);

        // dirty C code
        SDL_Renderer* sdl();
    };


    /// a graphic object allocated in the VRAM,
    class texture {
    public:
        friend class renderer;

        enum class access : int {
            static_ = SDL_TEXTUREACCESS_STATIC,
            streaming = SDL_TEXTUREACCESS_STREAMING,
            target = SDL_TEXTUREACCESS_TARGET
        };

        const pixelformat::format format;
        const access access_;

        texture() = delete;

        // TODO: create a texture copy constructor?
        texture(const texture& other) = delete;
        // TODO: create move constructor
        texture(texture&& other) = default;
        // TODO: create surface wrapper class
        // texture(const surface& surf);

        texture(renderer& r, pixelformat::format p, access a,
                std::size_t width, std::size_t height);
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

        /// lock a portion to be write-only
        // TODO: needs surface wrapper class
        // bool lock(const rect& region, const surface&);
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

    private:
        renderer& m_renderer;
        SDL_Texture *m_texture;

        // dirty C code
        SDL_Texture* sdl();
    };


    /// a basic wrapper around a SDL window
    class window {
    public:
        friend class renderer;
        friend class event::event;

        window() = delete;
        window(const window& other) = delete;

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
        renderer& get_renderer() { return m_renderer; }
        void update();

        static window& get(unsigned id);

    private:
        bool m_open;
        SDL_Window *m_window;
        const unsigned m_id;

        renderer m_renderer;

        // dirty C code
        SDL_Window* sdl();

        // code used by events
        static std::unordered_map<unsigned, window*> _windows;
    };
}
