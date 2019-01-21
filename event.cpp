#include "event.hpp"

#include <optional>

extern "C" {
#include <SDL2/SDL.h>
}

wrapsdl2::event::event(const SDL_Event& e) {
    m_event = e;
}

std::optional<wrapsdl2::event> wrapsdl2::poll_event() {
    SDL_Event ev;

    if (SDL_PollEvent(&ev) != 0) {
        return event(ev);
    }

    return std::nullopt;
}