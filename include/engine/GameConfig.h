#pragma once

#include <cstdint>

namespace minesweeper {

struct GameConfig {
    uint32_t width = 16;
    uint32_t height = 16;
    uint32_t mine_count = 40;
    uint32_t seed = 0;  // 0 means random seed
    
    GameConfig() = default;
    GameConfig(uint32_t w, uint32_t h, uint32_t mines, uint32_t s = 0)
        : width(w), height(h), mine_count(mines), seed(s) {}
    
    bool is_valid() const {
        return width > 0 && height > 0 && 
               mine_count > 0 && 
               mine_count < (width * height);
    }
};

enum class GameState : uint8_t {
    Ready,
    Playing,
    Won,
    Lost
};

} // namespace minesweeper
