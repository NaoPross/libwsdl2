#include "event.hpp"
#include "video.hpp"

extern "C" {
#include <SDL2/SDL.h>
}

using namespace wsdl2::event;

event_t::event_t(const SDL_Event& e) : m_event(e) {}

event_t::event_t(const event_t& e) : m_event(e.m_event) {}

SDL_EventType event_t::type() const
{
    return m_event.type;
}

std::shared_ptr<event_t> poll_event() {

    SDL_Event ev;

#define EV_PTR(__type__) std::make_shared<event_t>(ev)
    
    using namespace wsdl2::event;

    if (SDL_PollEvent(&ev) != 0) {

        switch (ev.type)
        {

        // keyboard events
        case SDL_KEYUP:
            return EV_PTR(e_key);
        case SDL_KEYDOWN:
            return EV_PTR(e_key);

        // mouse events
        case SDL_MOUSEMOTION:
            return EV_PTR(mouse::e_motion);
        case SDL_MOUSEBUTTONDOWN:
            return EV_PTR(mouse::e_button);
        case SDL_MOUSEBUTTONUP:
            return EV_PTR(mouse::e_button);
        case SDL_MOUSEWHEEL:
            return EV_PTR(mouse::e_wheel);

        // sdl quit event
        case SDL_QUIT:
            return EV_PTR(e_quit);

        // window events
        case SDL_WINDOWEVENT:

            switch (ev.window.event)
            {
            case (SDL_WINDOWEVENT_MOVED)
                return EV_PTR(window::e_move);
            case (SDL_WINDOWEVENT_RESIZED)
                return EV_PTR(window::e_resize);
            case (SDL_WINDOWEVENT_SIZE_CHANGED)
                return EV_PTR(window::e_resize);
            default:
                return EV_PTR(window::e_window);
            }
        }

        return nullptr;
    }

    return nullptr;
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
