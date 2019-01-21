#include "../debug.hpp"

#include "wrapsdl2.hpp"
#include "video.hpp"
#include "event.hpp"

#include <iostream>
#include <thread>
#include <mutex>

int main(int argc, char *argv[]) {

    using namespace wrapsdl2;

    wrapsdl2::initialize();

    window win("Window Test", 800, 600);
    std::mutex win_mutex;

    win.open();

    std::thread win_update([&]() {
        std::lock_guard<std::mutex> lock(win_mutex);
        do {
            std::optional<event> ev = poll_event();
            if (ev.has_value()) {
                event& event = ev.value();
                npdebug("received event", event.sdl().type);

                // TODO: remove this sdl code
                if (event.sdl().type == SDL_WINDOWEVENT) {
                    if (event.sdl().window.event == SDL_WINDOWEVENT_CLOSE) {
                        npdebug("SDL_WINDOWEVENT_CLOSE")
                        win.close();
                    }
                }

                if (event.sdl().type == SDL_QUIT) {
                    npdebug("SDL_QUIT");
                    win.close();
                }
            }

            win.update();
        } while (win.is_open());
    });

    win_update.join();

    wrapsdl2::quit();
    return 0;
}
