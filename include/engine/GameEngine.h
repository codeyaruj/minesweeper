#pragma once

#include "Cell.h"
#include "Move.h"
#include "GameConfig.h"
#include <vector>
#include <random>
#include <stack>
#include <string>

namespace minesweeper {

class GameEngine {
public:
    explicit GameEngine(const GameConfig& config);
    
    // Core game operations
    bool reveal(uint32_t x, uint32_t y);
    bool toggle_flag(uint32_t x, uint32_t y);
    bool undo();
    void reset();
    void new_game(const GameConfig& config);
    
    // State queries
    GameState get_state() const { return state_; }
    uint32_t get_width() const { return config_.width; }
    uint32_t get_height() const { return config_.height; }
    uint32_t get_mine_count() const { return config_.mine_count; }
    uint32_t get_flags_placed() const { return flags_placed_; }
    uint32_t get_revealed_count() const { return revealed_count_; }
    float get_elapsed_time() const { return elapsed_time_; }
    const Cell& get_cell(uint32_t x, uint32_t y) const;
    const std::vector<Cell>& get_board() const { return board_; }
    
    // Timing
    void update(float delta_time);
    
    // Serialization
    bool save_to_file(const std::string& filename) const;
    bool load_from_file(const std::string& filename);
    
    // Replay
    const std::vector<Move>& get_move_history() const { return move_history_; }
    bool replay_move(size_t index);
    
    // Debug
    void set_debug_mode(bool enabled) { debug_mode_ = enabled; }
    bool is_debug_mode() const { return debug_mode_; }
    uint32_t get_seed() const { return actual_seed_; }

private:
    void initialize_board();
    void place_mines(uint32_t safe_x, uint32_t safe_y);
    void calculate_adjacent_counts();
    void flood_fill_reveal(uint32_t start_x, uint32_t start_y);
    bool check_win_condition();
    void reveal_all_mines();
    
    uint32_t coord_to_index(uint32_t x, uint32_t y) const {
        return y * config_.width + x;
    }
    
    bool is_valid_coord(uint32_t x, uint32_t y) const {
        return x < config_.width && y < config_.height;
    }
    
    struct UndoState {
        std::vector<Cell> board_snapshot;
        GameState state;
        uint32_t flags_placed;
        uint32_t revealed_count;
        Move move;
    };

private:
    GameConfig config_;
    std::vector<Cell> board_;
    GameState state_;
    uint32_t flags_placed_;
    uint32_t revealed_count_;
    float elapsed_time_;
    bool first_click_;
    bool debug_mode_;
    
    std::mt19937 rng_;
    uint32_t actual_seed_;
    
    std::vector<Move> move_history_;
    std::stack<UndoState> undo_stack_;
    
    static constexpr int8_t DIRECTIONS[8][2] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
};

} // namespace minesweeper
