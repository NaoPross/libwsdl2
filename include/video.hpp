#pragma once

#include <string>

class SDL_Window;
class SDL_Surface;

namespace wrapsdl2 {
    class window {
    public:
        window() = delete;
        window(const window& other) = delete;

        window(const std::string& title, std::size_t width, std::size_t height);
        ~window();

        // manipulation
        void show();
        void hide();
        void raise();

        // rendering
        void update();

    private:
        SDL_Window *m_window;
        SDL_Surface *m_surface;
    };
}