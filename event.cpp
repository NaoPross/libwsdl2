#include "event.hpp"
#include "video.hpp"

#include <optional>

extern "C" {
#include <SDL2/SDL.h>
}

using namespace wsdl2::event;

event_t::event_t(const SDL_Event& e) {
    m_event = e;
}

SDL_EventType event_t::type() const
{
    return m_event.type;
}

std::optional<event_t> poll_event() {
    SDL_Event ev;

    if (SDL_PollEvent(&ev) != 0) {
        return event_t(ev);
    }

    return std::nullopt;
}


/*
 * Keyboard
 */

e_key::action_t e_key::action() const
{
    return static_cast<e_key::action_t>(sdl().type);
}

SDL_Keycode e_key::get() const
{
    return sdl().key.keysym.sym;
}

/*
 * Mouse
 */

mouse::action_t mouse::e_mouse::action() const
{
    return static_cast<mouse::action_t>(sdl().type);
}

mm::vec2<int> mouse::e_mouse::location() const;
{
    // TODO
    return mm::vec2<int>({});
}

// TODO other structures
//
