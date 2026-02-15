#include "engine/GameEngine.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <queue>

namespace minesweeper {

GameEngine::GameEngine(const GameConfig& config)
    : config_(config)
    , state_(GameState::Ready)
    , flags_placed_(0)
    , revealed_count_(0)
    , elapsed_time_(0.0f)
    , first_click_(true)
    , debug_mode_(false)
{
    if (!config_.is_valid()) {
        config_ = GameConfig();
    }
    
    if (config_.seed == 0) {
        actual_seed_ = static_cast<uint32_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );
    } else {
        actual_seed_ = config_.seed;
    }
    
    rng_.seed(actual_seed_);
    initialize_board();
}

void GameEngine::initialize_board() {
    board_.clear();
    board_.resize(config_.width * config_.height);
    state_ = GameState::Ready;
    flags_placed_ = 0;
    revealed_count_ = 0;
    elapsed_time_ = 0.0f;
    first_click_ = true;
    move_history_.clear();
    
    while (!undo_stack_.empty()) {
        undo_stack_.pop();
    }
}

void GameEngine::place_mines(uint32_t safe_x, uint32_t safe_y) {
    std::vector<uint32_t> available_positions;
    available_positions.reserve(board_.size());
    
    for (uint32_t y = 0; y < config_.height; ++y) {
        for (uint32_t x = 0; x < config_.width; ++x) {
            if (x == safe_x && y == safe_y) continue;
            
            bool is_adjacent = false;
            for (int i = 0; i < 8; ++i) {
                int32_t nx = static_cast<int32_t>(safe_x) + DIRECTIONS[i][0];
                int32_t ny = static_cast<int32_t>(safe_y) + DIRECTIONS[i][1];
                if (nx == static_cast<int32_t>(x) && ny == static_cast<int32_t>(y)) {
                    is_adjacent = true;
                    break;
                }
            }
            
            if (!is_adjacent) {
                available_positions.push_back(coord_to_index(x, y));
            }
        }
    }
    
    std::shuffle(available_positions.begin(), available_positions.end(), rng_);
    
    uint32_t mines_to_place = std::min(config_.mine_count, 
                                       static_cast<uint32_t>(available_positions.size()));
    
    for (uint32_t i = 0; i < mines_to_place; ++i) {
        board_[available_positions[i]].is_mine = true;
    }
    
    calculate_adjacent_counts();
}

void GameEngine::calculate_adjacent_counts() {
    for (uint32_t y = 0; y < config_.height; ++y) {
        for (uint32_t x = 0; x < config_.width; ++x) {
            uint32_t idx = coord_to_index(x, y);
            if (board_[idx].is_mine) continue;
            
            uint8_t count = 0;
            for (int i = 0; i < 8; ++i) {
                int32_t nx = static_cast<int32_t>(x) + DIRECTIONS[i][0];
                int32_t ny = static_cast<int32_t>(y) + DIRECTIONS[i][1];
                
                if (nx >= 0 && nx < static_cast<int32_t>(config_.width) &&
                    ny >= 0 && ny < static_cast<int32_t>(config_.height)) {
                    if (board_[coord_to_index(nx, ny)].is_mine) {
                        ++count;
                    }
                }
            }
            board_[idx].adjacent_count = count;
        }
    }
}

bool GameEngine::reveal(uint32_t x, uint32_t y) {
    if (state_ == GameState::Won || state_ == GameState::Lost) {
        return false;
    }
    
    if (!is_valid_coord(x, y)) {
        return false;
    }
    
    uint32_t idx = coord_to_index(x, y);
    Cell& cell = board_[idx];
    
    if (cell.revealed || cell.flagged) {
        return false;
    }
    
    if (first_click_) {
        place_mines(x, y);
        first_click_ = false;
        state_ = GameState::Playing;
    }
    
    UndoState undo_state;
    undo_state.board_snapshot = board_;
    undo_state.state = state_;
    undo_state.flags_placed = flags_placed_;
    undo_state.revealed_count = revealed_count_;
    undo_state.move = Move(x, y, MoveType::Reveal, elapsed_time_);
    undo_stack_.push(undo_state);
    
    move_history_.emplace_back(x, y, MoveType::Reveal, elapsed_time_);
    
    if (cell.is_mine) {
        cell.revealed = true;
        state_ = GameState::Lost;
        reveal_all_mines();
        return true;
    }
    
    flood_fill_reveal(x, y);
    
    if (check_win_condition()) {
        state_ = GameState::Won;
    }
    
    return true;
}

void GameEngine::flood_fill_reveal(uint32_t start_x, uint32_t start_y) {
    std::queue<std::pair<uint32_t, uint32_t>> queue;
    queue.push({start_x, start_y});
    
    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();
        
        if (!is_valid_coord(x, y)) continue;
        
        uint32_t idx = coord_to_index(x, y);
        Cell& cell = board_[idx];
        
        if (cell.revealed || cell.is_mine || cell.flagged) continue;
        
        cell.revealed = true;
        ++revealed_count_;
        
        if (cell.adjacent_count == 0) {
            for (int i = 0; i < 8; ++i) {
                int32_t nx = static_cast<int32_t>(x) + DIRECTIONS[i][0];
                int32_t ny = static_cast<int32_t>(y) + DIRECTIONS[i][1];
                
                if (nx >= 0 && nx < static_cast<int32_t>(config_.width) &&
                    ny >= 0 && ny < static_cast<int32_t>(config_.height)) {
                    queue.push({static_cast<uint32_t>(nx), static_cast<uint32_t>(ny)});
                }
            }
        }
    }
}

bool GameEngine::toggle_flag(uint32_t x, uint32_t y) {
    if (state_ == GameState::Won || state_ == GameState::Lost) {
        return false;
    }
    
    if (!is_valid_coord(x, y)) {
        return false;
    }
    
    uint32_t idx = coord_to_index(x, y);
    Cell& cell = board_[idx];
    
    if (cell.revealed) {
        return false;
    }
    
    if (first_click_) {
        state_ = GameState::Playing;
    }
    
    UndoState undo_state;
    undo_state.board_snapshot = board_;
    undo_state.state = state_;
    undo_state.flags_placed = flags_placed_;
    undo_state.revealed_count = revealed_count_;
    undo_state.move = Move(x, y, cell.flagged ? MoveType::Unflag : MoveType::Flag, elapsed_time_);
    undo_stack_.push(undo_state);
    
    if (cell.flagged) {
        cell.flagged = false;
        --flags_placed_;
        move_history_.emplace_back(x, y, MoveType::Unflag, elapsed_time_);
    } else {
        cell.flagged = true;
        ++flags_placed_;
        move_history_.emplace_back(x, y, MoveType::Flag, elapsed_time_);
    }
    
    return true;
}

bool GameEngine::undo() {
    if (undo_stack_.empty()) {
        return false;
    }
    
    UndoState undo_state = undo_stack_.top();
    undo_stack_.pop();
    
    board_ = undo_state.board_snapshot;
    state_ = undo_state.state;
    flags_placed_ = undo_state.flags_placed;
    revealed_count_ = undo_state.revealed_count;
    
    if (!move_history_.empty()) {
        move_history_.pop_back();
    }
    
    return true;
}

bool GameEngine::check_win_condition() {
    uint32_t non_mine_cells = config_.width * config_.height - config_.mine_count;
    return revealed_count_ >= non_mine_cells;
}

void GameEngine::reveal_all_mines() {
    for (auto& cell : board_) {
        if (cell.is_mine) {
            cell.revealed = true;
        }
    }
}

void GameEngine::reset() {
    initialize_board();
    if (config_.seed == 0) {
        actual_seed_ = static_cast<uint32_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );
    } else {
        actual_seed_ = config_.seed;
    }
    rng_.seed(actual_seed_);
}

void GameEngine::new_game(const GameConfig& config) {
    config_ = config;
    if (!config_.is_valid()) {
        config_ = GameConfig();
    }
    reset();
}

void GameEngine::update(float delta_time) {
    if (state_ == GameState::Playing) {
        elapsed_time_ += delta_time;
    }
}

const Cell& GameEngine::get_cell(uint32_t x, uint32_t y) const {
    return board_[coord_to_index(x, y)];
}

bool GameEngine::save_to_file(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    
    file.write(reinterpret_cast<const char*>(&config_.width), sizeof(config_.width));
    file.write(reinterpret_cast<const char*>(&config_.height), sizeof(config_.height));
    file.write(reinterpret_cast<const char*>(&config_.mine_count), sizeof(config_.mine_count));
    file.write(reinterpret_cast<const char*>(&actual_seed_), sizeof(actual_seed_));
    
    uint8_t state_val = static_cast<uint8_t>(state_);
    file.write(reinterpret_cast<const char*>(&state_val), sizeof(state_val));
    file.write(reinterpret_cast<const char*>(&flags_placed_), sizeof(flags_placed_));
    file.write(reinterpret_cast<const char*>(&revealed_count_), sizeof(revealed_count_));
    file.write(reinterpret_cast<const char*>(&elapsed_time_), sizeof(elapsed_time_));
    file.write(reinterpret_cast<const char*>(&first_click_), sizeof(first_click_));
    
    for (const auto& cell : board_) {
        file.write(reinterpret_cast<const char*>(&cell.is_mine), sizeof(cell.is_mine));
        file.write(reinterpret_cast<const char*>(&cell.revealed), sizeof(cell.revealed));
        file.write(reinterpret_cast<const char*>(&cell.flagged), sizeof(cell.flagged));
        file.write(reinterpret_cast<const char*>(&cell.adjacent_count), sizeof(cell.adjacent_count));
    }
    
    size_t move_count = move_history_.size();
    file.write(reinterpret_cast<const char*>(&move_count), sizeof(move_count));
    for (const auto& move : move_history_) {
        file.write(reinterpret_cast<const char*>(&move.x), sizeof(move.x));
        file.write(reinterpret_cast<const char*>(&move.y), sizeof(move.y));
        uint8_t move_type = static_cast<uint8_t>(move.type);
        file.write(reinterpret_cast<const char*>(&move_type), sizeof(move_type));
        file.write(reinterpret_cast<const char*>(&move.timestamp), sizeof(move.timestamp));
    }
    
    return file.good();
}

bool GameEngine::load_from_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;
    
    GameConfig new_config;
    file.read(reinterpret_cast<char*>(&new_config.width), sizeof(new_config.width));
    file.read(reinterpret_cast<char*>(&new_config.height), sizeof(new_config.height));
    file.read(reinterpret_cast<char*>(&new_config.mine_count), sizeof(new_config.mine_count));
    file.read(reinterpret_cast<char*>(&new_config.seed), sizeof(new_config.seed));
    
    if (!new_config.is_valid()) return false;
    
    config_ = new_config;
    actual_seed_ = config_.seed;
    
    uint8_t state_val;
    file.read(reinterpret_cast<char*>(&state_val), sizeof(state_val));
    state_ = static_cast<GameState>(state_val);
    file.read(reinterpret_cast<char*>(&flags_placed_), sizeof(flags_placed_));
    file.read(reinterpret_cast<char*>(&revealed_count_), sizeof(revealed_count_));
    file.read(reinterpret_cast<char*>(&elapsed_time_), sizeof(elapsed_time_));
    file.read(reinterpret_cast<char*>(&first_click_), sizeof(first_click_));
    
    board_.clear();
    board_.resize(config_.width * config_.height);
    
    for (auto& cell : board_) {
        file.read(reinterpret_cast<char*>(&cell.is_mine), sizeof(cell.is_mine));
        file.read(reinterpret_cast<char*>(&cell.revealed), sizeof(cell.revealed));
        file.read(reinterpret_cast<char*>(&cell.flagged), sizeof(cell.flagged));
        file.read(reinterpret_cast<char*>(&cell.adjacent_count), sizeof(cell.adjacent_count));
    }
    
    size_t move_count;
    file.read(reinterpret_cast<char*>(&move_count), sizeof(move_count));
    move_history_.clear();
    move_history_.reserve(move_count);
    
    for (size_t i = 0; i < move_count; ++i) {
        uint32_t x, y;
        uint8_t move_type;
        float timestamp;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        file.read(reinterpret_cast<char*>(&move_type), sizeof(move_type));
        file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        move_history_.emplace_back(x, y, static_cast<MoveType>(move_type), timestamp);
    }
    
    while (!undo_stack_.empty()) {
        undo_stack_.pop();
    }
    
    return file.good();
}

bool GameEngine::replay_move(size_t index) {
    if (index >= move_history_.size()) {
        return false;
    }
    
    const Move& move = move_history_[index];
    
    switch (move.type) {
        case MoveType::Reveal:
            return reveal(move.x, move.y);
        case MoveType::Flag:
        case MoveType::Unflag:
            return toggle_flag(move.x, move.y);
    }
    
    return false;
}

} // namespace minesweeper
