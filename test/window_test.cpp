#include "wrapsdl2.hpp"
#include "video.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
    wrapsdl2::initialize();

    {
        wrapsdl2::window win("Window Test", 800, 600);

        std::cout << "press ENTER to show the window" << std::endl;
        std::cin.get();

        win.show();
        win.update();

        std::cout << "press ENTER to quit" << std::endl;
        std::cin.get();
    }

    wrapsdl2::quit();
    return 0;
}
