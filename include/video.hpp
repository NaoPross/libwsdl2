#pragma once

#include "util.hpp"

#include <string>
#include <array>
#include <type_traits>

extern "C" {
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
}

namespace wsdl2 {
    // forward declarations
    class window;

    // name aliases
    using rect = SDL_Rect;
    using point = SDL_Point;

    struct color {
        std::uint8_t r, g, b, a;
    };

    class renderer {
    public:
        friend class window;

        renderer(window& w);
        virtual ~renderer();

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

        inline void draw_rect(const rect& rect) {
            util::check(0 == SDL_RenderDrawRect(sdl(), &rect));
        }

        inline void fill_rect(const rect& rect) {
            util::check(0 == SDL_RenderFillRect(sdl(), &rect));
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

        inline void draw(const point& p) { draw_point(p); }
        inline void draw(const rect& r)  { draw_rect(r); }
        inline void fill(const rect& r)  { fill_rect(r); }

        template<template<typename> typename Container>
        inline void draw(const Container<point>& points) {
            draw_points(points);
        }

        template<template<typename> typename Container>
        inline void draw(const Container<rect>& rects) {
            draw_rects(rects);
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

    class window {
    public:
        friend class renderer;

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

    private:
        bool m_open;
        renderer m_renderer;
        SDL_Window *m_window;

        // dirty C code
        SDL_Window* sdl();
    };



}