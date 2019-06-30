#include "wsdl2/debug.hpp"

#include "wsdl2/wsdl2.hpp"
#include "wsdl2/video.hpp"
#include "wsdl2/event.hpp"

#include <iostream>
#include <thread>
#include <mutex>
#include <type_traits>

int main() {

    using namespace wsdl2;

    wsdl2::initialize();

    window win("Window Test", 800, 600);
    std::mutex win_mutex;

    win.open();

    std::thread win_update([&]() {
        std::lock_guard<std::mutex> lock(win_mutex);
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

            win.clear();
            win.present();
            // ~60 fps test
            wsdl2::delay(static_cast<unsigned>(1000.0/60.0));
        } while (win.is_open());
    });

    win_update.join();

    wsdl2::quit();
    return 0;
}
