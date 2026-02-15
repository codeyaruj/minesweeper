#pragma once

#include <cstdint>

namespace minesweeper {

enum class MoveType : uint8_t {
    Reveal,
    Flag,
    Unflag
};

struct Move {
    uint32_t x;
    uint32_t y;
    MoveType type;
    float timestamp;

    Move() : x(0), y(0), type(MoveType::Reveal), timestamp(0.0f) {}
    
    Move(uint32_t x_, uint32_t y_, MoveType type_, float time = 0.0f)
        : x(x_), y(y_), type(type_), timestamp(time) {}
};

} // namespace minesweeper
