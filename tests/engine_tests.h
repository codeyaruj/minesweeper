#pragma once

// Example unit tests for GameEngine
// To use: integrate with GoogleTest or Catch2

#include "engine/GameEngine.h"
#include <cassert>

namespace minesweeper {
namespace tests {

void test_initialization() {
    GameConfig config(10, 10, 15, 42);
    GameEngine engine(config);
    
    assert(engine.get_width() == 10);
    assert(engine.get_height() == 10);
    assert(engine.get_mine_count() == 15);
    assert(engine.get_state() == GameState::Ready);
    assert(engine.get_flags_placed() == 0);
    assert(engine.get_revealed_count() == 0);
}

void test_first_click_safety() {
    GameConfig config(10, 10, 50, 42);
    GameEngine engine(config);
    
    // First click should never be a mine
    engine.reveal(5, 5);
    assert(!engine.get_cell(5, 5).is_mine);
    assert(engine.get_state() == GameState::Playing);
}

void test_flag_toggle() {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);
    
    engine.toggle_flag(0, 0);
    assert(engine.get_cell(0, 0).flagged);
    assert(engine.get_flags_placed() == 1);
    
    engine.toggle_flag(0, 0);
    assert(!engine.get_cell(0, 0).flagged);
    assert(engine.get_flags_placed() == 0);
}

void test_undo_functionality() {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);
    
    engine.toggle_flag(0, 0);
    assert(engine.get_cell(0, 0).flagged);
    
    engine.undo();
    assert(!engine.get_cell(0, 0).flagged);
}

void test_save_load() {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);
    
    engine.reveal(5, 5);
    engine.toggle_flag(0, 0);
    
    assert(engine.save_to_file("test_save.dat"));
    
    GameEngine engine2(GameConfig());
    assert(engine2.load_from_file("test_save.dat"));
    
    assert(engine2.get_width() == 10);
    assert(engine2.get_height() == 10);
    assert(engine2.get_cell(0, 0).flagged);
}

void test_deterministic_generation() {
    GameConfig config1(10, 10, 10, 42);
    GameConfig config2(10, 10, 10, 42);
    
    GameEngine engine1(config1);
    GameEngine engine2(config2);
    
    // Same seed should produce identical boards
    engine1.reveal(5, 5);
    engine2.reveal(5, 5);
    
    for (uint32_t y = 0; y < 10; ++y) {
        for (uint32_t x = 0; x < 10; ++x) {
            const Cell& cell1 = engine1.get_cell(x, y);
            const Cell& cell2 = engine2.get_cell(x, y);
            assert(cell1.is_mine == cell2.is_mine);
            assert(cell1.adjacent_count == cell2.adjacent_count);
        }
    }
}

void test_win_condition() {
    // Small board with few mines for easy testing
    GameConfig config(3, 3, 1, 42);
    GameEngine engine(config);
    
    // Reveal all non-mine cells
    for (uint32_t y = 0; y < 3; ++y) {
        for (uint32_t x = 0; x < 3; ++x) {
            if (!engine.get_cell(x, y).is_mine) {
                engine.reveal(x, y);
            }
        }
    }
    
    // Should win when all non-mine cells are revealed
    if (engine.get_revealed_count() == 8) {
        assert(engine.get_state() == GameState::Won);
    }
}

void test_move_history() {
    GameConfig config(10, 10, 10, 42);
    GameEngine engine(config);
    
    engine.reveal(5, 5);
    engine.toggle_flag(0, 0);
    engine.toggle_flag(1, 1);
    
    const auto& history = engine.get_move_history();
    assert(history.size() == 3);
    assert(history[0].type == MoveType::Reveal);
    assert(history[1].type == MoveType::Flag);
    assert(history[2].type == MoveType::Flag);
}

void run_all_tests() {
    test_initialization();
    test_first_click_safety();
    test_flag_toggle();
    test_undo_functionality();
    test_save_load();
    test_deterministic_generation();
    test_win_condition();
    test_move_history();
}

} // namespace tests
} // namespace minesweeper
