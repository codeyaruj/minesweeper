#include "core/Application.h"
#include <iostream>
#include <exception>

int main(int, char**) {
    try {
        minesweeper::Application app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
