#include "../debug.hpp"

#include "wsdl2.hpp"
#include "video.hpp"
#include "event.hpp"

#include <iostream>
#include <thread>
#include <mutex>

int main(int argc, char *argv[]) {

    using namespace wsdl2;

    wsdl2::initialize();

    window win("Window Test", 800, 600);
    std::mutex win_mutex;

    win.open();

    std::thread win_update([&]() {
        std::lock_guard<std::mutex> lock(win_mutex);
        do {

            win.update();
            // ~60 fps test
            wsdl2::delay(1000.0/60.0);
        } while (win.is_open());
    });

    win_update.join();

    wsdl2::quit();
    return 0;
}
