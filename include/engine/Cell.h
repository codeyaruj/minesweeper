#pragma once

#include <cstdint>

namespace minesweeper {

struct Cell {
    bool is_mine = false;
    bool revealed = false;
    bool flagged = false;
    uint8_t adjacent_count = 0;

    Cell() = default;
    
    void reset() {
        is_mine = false;
        revealed = false;
        flagged = false;
        adjacent_count = 0;
    }
};

} // namespace minesweeper
