#pragma once

#include <string>
#include <array>

extern "C" {
#include <SDL2/SDL_video.h>
}

namespace wrapsdl2 {
    typedef SDL_Point point;
    typedef SDL_Rect rect;

    class window {
    public:
        window() = delete;
        window(const window& other) = delete;

        window(const std::string& title, std::size_t width, std::size_t height);
        ~window();

        // setters
        void show();
        void hide();
        void raise();

        // getters
        bool visible();

        // rendering
        void update();

    private:
        SDL_Window *m_window;
        SDL_Surface *m_surface;
    };
}