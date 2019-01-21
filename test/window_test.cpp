#include "wrapsdl2.hpp"
#include "video.hpp"

#include <iostream>
#include <thread>
#include <mutex>

int main(int argc, char *argv[]) {
    wrapsdl2::initialize();

    {
        using namespace wrapsdl2;

        window win("Window Test", 800, 600);
        std::mutex win_mutex;

        std::cout << "press ENTER to show the window" << std::endl;
        std::cin.get();

        win.show();

        std::thread win_update([&]() {
            std::lock_guard<std::mutex> lock(win_mutex);
            while (win.is_open()) {
                win.update();
                wrapsdl2::delay(200);


            }
        });

        std::cout << "press ENTER to quit" << std::endl;
        std::cin.get();

        win_update.join();
    }

    wrapsdl2::quit();
    return 0;
}
