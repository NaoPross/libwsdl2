#include "../debug.hpp"

#include "wsdl2.hpp"
#include "video.hpp"
#include "event.hpp"

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
                std::visit([&](auto& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    
                    if constexpr (std::is_same_v<T, event::quit>) {
                        win.close();
                    }
                }, *event);
            }

            win.update();
            // ~60 fps test
            wsdl2::delay(static_cast<unsigned>(1000.0/60.0));
        } while (win.is_open());
    });

    win_update.join();

    wsdl2::quit();
    return 0;
}
