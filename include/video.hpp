#pragma once

#include <string>
#include <array>

extern "C" {
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
}

namespace wsdl2 {
    typedef SDL_Point point;
    typedef SDL_Rect rect;

    class window;

    class renderer {
    public:
        friend class window;

        renderer(window& w);
        virtual ~renderer();

        inline void clear() { SDL_RenderClear(safe()); }
        inline void present() { SDL_RenderPresent(safe()); }

        // dirty C code
        inline SDL_Renderer* sdl() { return m_renderer; }

    private:
        renderer();

        SDL_Renderer* safe();
        void create_sdl_renderer(SDL_Window *win);

        SDL_Renderer *m_renderer;
    };

    class window {
    public:
        window() = delete;
        window(const window& other) = delete;

        window(const std::string& title, std::size_t width, std::size_t height);
        virtual ~window();

        // setters
        void open();
        void close();

        inline void show() { SDL_ShowWindow(m_window); }
        inline void hide() { SDL_HideWindow(m_window); }
        inline void raise() { SDL_RaiseWindow(m_window); }

        // getters
        bool is_open();
        bool is_visible();

        // rendering
        void update();

        // dirty C code
        inline SDL_Window* sdl() { return m_window; }

    private:
        bool m_open;
        renderer m_renderer;
        SDL_Window *m_window;
    };



}