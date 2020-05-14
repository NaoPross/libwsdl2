#pragma once

#ifdef WSDL2_USE_MM
#include "mm/mmvec.hpp"
#endif 

#include <memory>
#include <optional>
#include <variant>

#ifdef DEBUG
#include <cassert>
#endif

#ifndef __WSDL_EVENT_H__
#define __WSDL_EVENT_H__

extern "C" {
#include <SDL2/SDL_events.h>
}

namespace wsdl2 {

    // forward declaration
    class window;

    enum class button {
        left = SDL_BUTTON_LEFT,
        middle = SDL_BUTTON_MIDDLE,
        right = SDL_BUTTON_RIGHT,
        x1 = SDL_BUTTON_X1,
        x2 = SDL_BUTTON_X2 
    };
}

namespace wsdl2::event {
    // forward declarations of events
    class quit;
    class key;
    namespace mouse {
        class button;
        class motion;
        class wheel;
    }

    namespace window {
        class shown;
        class hidden;
        class exposed;
        class moved;
        class resized;
    }

    namespace helper {
        // TODO: deprecated, remove soon
        // UNUSED wrapper around SDL_Event
        class event {
        private:
            SDL_Event m_event;

        protected:
            SDL_Event& sdl() { return m_event; }
            const SDL_Event& sdl() const { return m_event; }

        public:
            event(SDL_Event&& e);
            virtual ~event() {}

            uint32_t type() const;
        };
    }

    struct quit {
        std::uint32_t __type;
        std::uint32_t timestamp;

        static inline quit from_event(const SDL_Event& e) {
            static_assert(sizeof(quit) == sizeof(SDL_QuitEvent));
            static_assert(alignof(quit) == alignof(SDL_QuitEvent));
#ifdef DEBUG
            assert(e.type == SDL_QUIT);
#endif
            // TODO: test the shit out of this
            return *reinterpret_cast<const quit*>(&e.quit);
        }
    };

    struct key {
        using keycode = SDL_Keycode;
        using scancode = SDL_Scancode;
        using symbol = SDL_Keysym;

        enum class action : std::uint32_t {
            up = SDL_KEYUP, 
            down = SDL_KEYDOWN,
        };

        // TODO, increase user-friendlyness

        const action            type; 
        const std::uint32_t     timestamp;
        const std::uint32_t     window_id;
        const std::uint8_t      __state;
        const std::uint8_t      repeat;

    private:
        const symbol keysym;

    public:

        key() = delete;

        keycode code() const {
            
            return keysym.sym;
        }

        scancode phys_code() const {

            return keysym.scancode;
        }

        static inline key from_event(const SDL_Event& e) {
            static_assert(sizeof(key) == sizeof(SDL_KeyboardEvent));
            static_assert(alignof(key) == alignof(SDL_KeyboardEvent));

#ifdef DEBUG
            assert((e.type == SDL_KEYDOWN) || (e.type == SDL_KEYUP));
#endif
            // TODO: test the shit out of this
            return *reinterpret_cast<const key*>(&e.key);
        }
    };

    namespace mouse {
        struct button {
            enum class action : std::uint32_t {
                up = SDL_MOUSEBUTTONUP,
                down = SDL_MOUSEBUTTONDOWN,
            };

            const action            type;
            const std::uint32_t     timestamp;
            const std::uint32_t     window_id;
            const wsdl2::button     btn;
            const std::uint8_t      clicks;

            const std::int32_t x;
            const std::int32_t y;

            button() = delete;

            static inline button from_event(const SDL_Event& e) {
                static_assert(sizeof(button) == sizeof(SDL_MouseButtonEvent));
                static_assert(alignof(button) == alignof(SDL_MouseButtonEvent));

#ifdef DEBUG
                assert((e.type == SDL_MOUSEBUTTONUP) 
                    || (e.type == SDL_MOUSEBUTTONDOWN));
#endif

                // TODO: test the shit out of this
                return *reinterpret_cast<const button*>(&e.button);
            }

#ifdef WSDL2_USE_MM
            mm::vec2<int> where() const {
                return {x, y};
            }
#endif
        };

        struct motion {
            std::uint32_t __type;
            std::uint32_t timestamp;
            std::uint32_t window_id;
            std::uint32_t __which;
            std::uint32_t __state;

            std::int32_t  x;
            std::int32_t  y;
            std::int32_t  xrel;
            std::int32_t  yrel;

            // TODO: make helpers to read MASKS
            // SDL_BUTTON_LMASK
            // SDL_BUTTON_MMASK
            // SDL_BUTTON_RMASK
            // SDL_BUTTON_X1MASK
            // SDL_BUTTON_X2MASK
            // void was_pressed(wsdl2::button b);

            motion() = delete;

            static inline motion from_event(const SDL_Event& e) {
                static_assert(sizeof(motion) == sizeof(SDL_MouseMotionEvent));
                static_assert(alignof(motion) == alignof(SDL_MouseMotionEvent));

#ifdef DEBUG
                assert(e.type == SDL_MOUSEMOTION);
#endif
                // TODO: test the shit out of this
                return *reinterpret_cast<const motion*>(&e.motion);
            }

#ifdef WSDL2_USE_MM
            mm::vec2<int> where() const {
                return {x, y};
            }

            mm::vec2<int> delta() const {
                return { xrel, yrel };
            }
#endif
        };

        struct wheel {
            enum class scroll_direction : std::uint32_t {
                normal = SDL_MOUSEWHEEL_NORMAL,
                flipped = SDL_MOUSEWHEEL_FLIPPED,
            };

            const std::uint32_t __type;
            const std::uint32_t timestamp;
            const std::uint32_t window_id;
            const std::uint32_t __which;

            const std::int32_t  x;
            const std::int32_t  y;

            const scroll_direction direction;

            wheel() = delete;
            static inline wheel from_event(const SDL_Event& e) {
                static_assert(sizeof(wheel) == sizeof(SDL_MouseWheelEvent));
                static_assert(alignof(wheel) == alignof(SDL_MouseWheelEvent));

#ifdef DEBUG
                assert(e.type == SDL_MOUSEWHEEL);
#endif
                // TODO: test the shit out of this
                return *reinterpret_cast<const wheel*>(&e.wheel);
            }

            /// return x or y checking the direction
            std::int32_t get_x() const {
                return (direction == scroll_direction::normal) ? x : x * -1;
            }

            std::int32_t get_y() const {
                return (direction == scroll_direction::normal) ? y : y * -1;
            }
        };
    }

    namespace window {
        enum class action : std::uint8_t {
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

        namespace helper {
            template<typename Derived, action Action>
            struct partial {
                std::uint32_t __type;
                std::uint32_t timestamp;
                std::uint32_t window_id;
                action        __action;

                partial() = delete;
                static inline Derived from_event(const SDL_Event& e) {
                    // TODO: fix these checks
                    // static_assert(sizeof(Derived) == sizeof(SDL_WindowEvent));
                    // static_assert(alignof(Derived) == alignof(SDL_WindowEvent));

#ifdef DEBUG
                    assert(e.type == SDL_WINDOWEVENT);
                    assert(static_cast<action>(e.window.event) == Action);
#endif
                    // TODO: test the shit out of this
                    return *reinterpret_cast<const Derived*>(&e.window);
                }
            };

            template<typename Derived, action Action>
            struct dataless : partial<Derived, Action> {
                std::int32_t  __pad1;
                std::int32_t  __pad2;
            };
        }

        struct shown : helper::dataless<shown, action::shown> {};
        struct hidden : helper::dataless<hidden, action::hidden> {};
        struct exposed : helper::dataless<hidden, action::exposed> {};

        struct moved : helper::partial<moved, action::moved> {
            std::int32_t  x;
            std::int32_t  y;
        };

        struct resized : helper::partial<resized, action::resized> {
            std::int32_t  width;
            std::int32_t  height;
        };
    }

    // this function cannot be placed inside the cpp because of some
    // weird template shit
    inline std::optional<std::variant<
        // quit event
        quit,
        // keyboard events
        key,
        // mouse events
        mouse::button,
        mouse::motion,
        mouse::wheel,
        // window events
        window::shown,
        window::hidden,
        window::exposed,
        window::moved,
        window::resized
    >> poll() {
        SDL_Event ev;
        
        if (SDL_PollEvent(&ev) != 0) {
            switch (ev.type) {
            // keyboard events
            case SDL_KEYUP: [[fallthrough]];
            case SDL_KEYDOWN:
                return key::from_event(ev);

            // mouse events
            case SDL_MOUSEBUTTONDOWN: [[fallthrough]];
            case SDL_MOUSEBUTTONUP:
                return mouse::button::from_event(ev);

            case SDL_MOUSEMOTION:
                return mouse::motion::from_event(ev);
                
            case SDL_MOUSEWHEEL:
                return mouse::wheel::from_event(ev);
     
            // sdl quit event
            case SDL_QUIT:
                return quit::from_event(ev);
     
            // window events
            case SDL_WINDOWEVENT:
                switch (ev.window.event) {
                case SDL_WINDOWEVENT_SHOWN:
                    return window::shown::from_event(ev);

                case SDL_WINDOWEVENT_HIDDEN:
                    return window::hidden::from_event(ev);

                case SDL_WINDOWEVENT_EXPOSED:
                    return window::exposed::from_event(ev);
                    
                case SDL_WINDOWEVENT_MOVED:
                    return window::moved::from_event(ev);

                case SDL_WINDOWEVENT_RESIZED:
                    return window::resized::from_event(ev);
                }
            }
        }

        return std::nullopt;
    }
}

#endif

