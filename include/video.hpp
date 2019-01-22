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

    class window {
    public:
        window() = delete;
        window(const window& other) = delete;

        window(const std::string& title, std::size_t width, std::size_t height);
        ~window();

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

    private:
        bool m_open;
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
    };
}