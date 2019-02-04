#pragma once

#include "mm/mmvec.hpp"
#include <optional>

extern "C" {
#include <SDL2/SDL_events.h>
}

namespace wsdl2 {

    // forward declaration
    class window;
}

namespace wsdl2::event {

    class event_t {

        SDL_Event m_event;

    protected:

        SDL_Event& sdl() { return m_event; }

        const SDL_Event& sdl() const { return m_event; }

   public:

        event_t(const SDL_Event& e);

        // copy constructor
        event_t(const event_t& e);

        SDL_EventType type() const;
    };

    struct e_key : public event_t
    {
        enum class action_t : unsigned
        {
            up = SDL_KEYUP, 
            down = SDL_KEYDOWN
        };

        using event_t::event_t;
        using event_t::event_t(const event_t& e);

        action_t action() const;

        // key pressed or released
        SDL_Keycode get() const;
    };

    namespace mouse {

        enum class action_t : unsigned
        {
            button_up = SDL_MOUSEBUTTONUP,
            button_down = SDL_MOUSEBUTTONDOWN, 
            motion = SDL_MOUSEMOTION, 
            wheel = SDL_MOUSEWHEEL
        };

        struct e_mouse : public event_t
        {
            virtual ~e_mouse() {}

            using event_t::event_t;
            using event_t::event_t(const event_t& e);

            action_t action() const;

            virtual mm::vec2<int> location() const = 0;
        };

        struct e_button : public e_mouse
        {
            using e_mouse::e_mouse;
            using event_t::event_t(const event_t& e);

            enum class button_t : unsigned
            {
                left = SDL_BUTTON_LEFT,
                middle = SDL_BUTTON_MIDDLE,
                right = SDL_BUTTON_RIGHT,
                x1 = SDL_BUTTON_X1,
                x2 = SDL_BUTTON_X2 
            };

            mm::vec2<int> clicks() const;

            button_t which() const;

            virtual mm::vec2<int> location() const override;
        };

        struct e_motion : public e_mouse 
        {
            using e_mouse::e_mouse;
            using event_t::event_t(const event_t& e);

            // delta (x, y)
            mm:vec2<int> movement() const;

            virtual mm::vec2<int> location() const override;
        };

        struct e_wheel : public e_mouse
        {
            using e_mouse::e_mouse;
            using event_t::event_t(const event_t& e);

            int horizontal() const;
            int vertical() const;

            virtual mm::vec2<int> location() const override;
        };

    }

    struct e_quit : public event_t
    {
        using event_t::event_t;
        using event_t::event_t(const event_t& e);
    };

    namespace window {

        enum class action_t : unsigned
        {
            shown = SDL_WINDOWEVENT_SHOWN, 
            hidden = SDL_WINDOWEVENT_HIDDEN, 
            exposed = SDL_WINDOWEVENT_EXPOSED, 
            moved = SDL_WINDOWEVENT_MOVED, 
            resized = SDL_WINDOWEVENT_RESIZED,
            size_changed = SDL_WINDOWEVENT_SIZE_CHANGED, 
            minimized = SDL_WINDOWEVENT_MINIMIZED, 
            maximized = SDL_WINDOWEVENT_MAXIMIZED, 
            restored = SDL_WINDOWEVENT_RESTORED, 
            enter = SDL_WINDOWEVENT_ENTER, 
            leave = SDL_WINDOWEVENT_LEAVE, 
            focus_gained = SDL_WINDOWEVENT_FOCUS_GAINED,
            focus_lost = SDL_WINDOWEVENT_FOCUS_LOST, 
            close = SDL_WINDOWEVENT_CLOSE, 
            take_focus = SDL_WINDOWEVENT_TAKE_FOCUS, 
            hit_test = SDL_WINDOWEVENT_HIT_TEST
        };


        struct e_window : public event_t
        {

            using event_t::event_t;
            using event_t::event_t(const event_t& e);

            // TODO, mapping sdl window id with wsdl2 object
            wsdl2::window * window();

            action_t action() const;
        };

        // window positional event
        struct e_move : public e_window
        {
            using e_window::e_window;
            using event_t::event_t(const event_t& e);

            mm::vec2<uint32_t> position();
        };

        // window bound event
        struct e_resize : public e_window
        {
            using e_window::e_window;
            using event_t::event_t(const event_t& e);

            mm::vec2<uint32_t> size();
        };
    }

    // TODO other handlers

    std::shared_ptr<event_t> poll_event();
}
