#pragma once

#include <optional>

extern "C" {
#include <SDL2/SDL_events.h>
}

namespace wsdl2 {
    class event {
    public:
        using type = SDL_EventType;

        event(const SDL_Event& e);

        SDL_Event& sdl() { return m_event; }

    private:
        SDL_Event m_event;
    };


    std::optional<event> poll_event();
}