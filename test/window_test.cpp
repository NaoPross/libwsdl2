#include "debug.hpp"

#include "wsdl2.hpp"
#include "video.hpp"
#include "event.hpp"

#include <iostream>
#include <type_traits>

int main() {

    using namespace wsdl2;

    wsdl2::initialize();

    window win("Window Test", 800, 600);
    win.open();

    do {
        auto event = event::poll();
        if (event) {
            std::visit([&](auto& e) {
                using T = std::decay_t<decltype(e)>;
                
                if constexpr (std::is_same_v<T, event::quit>) {
                    win.close();
                }
                
                if constexpr (std::is_same_v<T, event::key>) {
                    if (e.type == event::key::action::up) {
                        // TODO: map SDLKs and SDL_SCANCODEs
                        if (e.keysym.sym == SDLK_ESCAPE) {
                            win.close();
                        }
                    }
                }

                if constexpr (std::is_same_v<T, event::mouse::button>) {
                    if (e.type == event::mouse::button::action::down) {
                        std::cout << "you clicked at ("
                                  << e.x << ", " << e.y
                                  << ")\n";
                    }
                }
            }, *event);
        }

        win.update();
        // ~60 fps test
        wsdl2::delay(static_cast<unsigned>(1000.0/60.0));
    } while (win.is_open());

    wsdl2::quit();
    return 0;
}
