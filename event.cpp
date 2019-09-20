#include "wsdl2/event.hpp"

using namespace wsdl2::event;

// this function cannot be placed inside the cpp because of some
    // weird template shit
/*    std::optional<std::variant<
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
            case SDL_KEYUP: [[fallthrough]]
            case SDL_KEYDOWN:
                return key::from_event(ev);

            // mouse events
            case SDL_MOUSEBUTTONDOWN: [[fallthrough]]
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
*/
