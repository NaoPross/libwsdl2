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
    using point = SDL_Point;

    struct color {
        std::uint8_t r, g, b, a;
    };

    enum class blend_mode {
        // TODO: map blend modes
        unimplemented
    };

    /// a graphic object allocated in the VRAM,
    /// the CPU has not much control of this buffer
    class texture {
    public:
        enum class pixformat : Uint32 {
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

        enum class access : int {
            static_ = SDL_TEXTUREACCESS_STATIC,
            streaming = SDL_TEXTUREACCESS_STREAMING,
            target = SDL_TEXTUREACCESS_TARGET
        };

        const pixformat _format;
        const access _access;

        texture() = delete;

        // TODO: create a texture copy constructor?
        texture(const texture& other) = delete;
        // TODO: create surface wrapper class
        // texture(const surface& surf);

        texture(renderer& r, pixformat p, access a, std::size_t width, std::size_t height);
        virtual ~texture();

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

            util::check(supported);
            return false;

        }

        inline std::uint8_t alpha() {
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

    /// the guy who does the actual hard stuff
    class renderer {
    public:
        friend class window;
        friend class texture;

        renderer(window& w);
        renderer(texture& t);
        virtual ~renderer();

        inline void set_target() {}
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


    private:
        SDL_Renderer *m_renderer;

        renderer();
        void create_sdl_renderer(SDL_Window *win);

        // dirty C code
        SDL_Renderer* sdl();
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
