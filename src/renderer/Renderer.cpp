#include "renderer/Renderer.h"
#include <raylib.h>
#include <sstream>
#include <iomanip>

namespace minesweeper {

static Color ToRaylibColor(const MSColor& c) {
    return Color{c.r, c.g, c.b, c.a};
}

Renderer::Renderer(const RenderConfig& config)
    : config_(config)
    , tile_size_(config.tile_size)
    , board_offset_x_(0)
    , board_offset_y_(0)
    , show_debug_overlay_(false)
    , show_fps_(false)
    , message_timer_(0.0f)
    , message_duration_(0.0f)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    if (config_.vsync) {
        SetConfigFlags(FLAG_VSYNC_HINT);
    }

    InitWindow(config_.window_width, config_.window_height, config_.title.c_str());
    SetTargetFPS(60);
}

Renderer::~Renderer() {
    CloseWindow();
}

bool Renderer::should_close() const {
    return WindowShouldClose();
}

void Renderer::begin_frame() {
    BeginDrawing();
    ClearBackground(ToRaylibColor(MSColor(40, 44, 52)));
}

void Renderer::end_frame() {
    render_message();

    if (show_fps_) {
        DrawFPS(10, 10);
    }

    EndDrawing();
}

void Renderer::render(const GameEngine& engine) {
    calculate_board_position(engine);
    render_board(engine);
    render_ui_panel(engine);

    if (show_debug_overlay_) {
        render_debug_overlay(engine);
    }
}

void Renderer::calculate_board_position(const GameEngine& engine) {
    int32_t window_width = GetScreenWidth();
    int32_t window_height = GetScreenHeight();

    int32_t board_width = engine.get_width() * tile_size_;
    int32_t board_height = engine.get_height() * tile_size_;

    board_offset_x_ = (window_width - board_width) / 2;
    board_offset_y_ = (window_height - config_.ui_panel_height - board_height) / 2 + config_.ui_panel_height;
}

void Renderer::render_board(const GameEngine& engine) {
    uint32_t hovered_x = 0, hovered_y = 0;
    bool has_hover = get_hovered_cell(engine, hovered_x, hovered_y);

    for (uint32_t y = 0; y < engine.get_height(); ++y) {
        for (uint32_t x = 0; x < engine.get_width(); ++x) {
            const Cell& cell = engine.get_cell(x, y);
            bool is_hovered = has_hover && (x == hovered_x && y == hovered_y);
            render_cell(cell, x, y, is_hovered, engine.is_debug_mode());
        }
    }
}

MSColor Renderer::get_number_color(uint8_t count) {
    switch (count) {
        case 1: return MSColor(41, 128, 185);   // Blue
        case 2: return MSColor(39, 174, 96);    // Green
        case 3: return MSColor(231, 76, 60);    // Red
        case 4: return MSColor(142, 68, 173);   // Purple
        case 5: return MSColor(230, 126, 34);   // Orange
        case 6: return MSColor(26, 188, 156);   // Cyan
        case 7: return MSColor(44, 62, 80);     // Dark gray
        case 8: return MSColor(127, 140, 141);  // Light gray
        default: return MSColor(255, 255, 255);
    }
}

void Renderer::render_cell(const Cell& cell, uint32_t x, uint32_t y, bool hovered, bool debug_mode) {
    float screen_x = board_offset_x_ + x * tile_size_;
    float screen_y = board_offset_y_ + y * tile_size_;

    float inner_x = screen_x + CELL_PADDING;
    float inner_y = screen_y + CELL_PADDING;
    float inner_size = tile_size_ - CELL_PADDING * 2;

    MSColor base_color = cell.revealed ? MSColor(52, 73, 94) : MSColor(149, 165, 166);

    if (hovered && !cell.revealed) {
        base_color = MSColor(127, 140, 141);
    }

    if (debug_mode && cell.is_mine && !cell.revealed) {
        base_color = MSColor(231, 76, 60, 100);
    }

    DrawRectangle(screen_x, screen_y, tile_size_, tile_size_, ToRaylibColor(MSColor(44, 62, 80)));
    DrawRectangle(inner_x, inner_y, inner_size, inner_size, ToRaylibColor(base_color));

    if (cell.revealed) {
        if (cell.is_mine) {
            float mine_size = inner_size * 0.5f;
            float mine_x = inner_x + (inner_size - mine_size) / 2;
            float mine_y = inner_y + (inner_size - mine_size) / 2;
            DrawCircle(mine_x + mine_size / 2, mine_y + mine_size / 2, mine_size / 2,
                      ToRaylibColor(MSColor(0, 0, 0)));
        } else if (cell.adjacent_count > 0) {
            MSColor num_color = get_number_color(cell.adjacent_count);

            std::string text = std::to_string(cell.adjacent_count);
            int font_size = tile_size_ / 2;
            int text_width = MeasureText(text.c_str(), font_size);

            DrawText(text.c_str(),
                    inner_x + (inner_size - text_width) / 2,
                    inner_y + (inner_size - font_size) / 2,
                    font_size,
                    ToRaylibColor(num_color));
        }
    } else if (cell.flagged) {
        float flag_width = inner_size * 0.6f;
        float flag_height = inner_size * 0.7f;
        float flag_x = inner_x + (inner_size - flag_width) / 2;
        float flag_y = inner_y + (inner_size - flag_height) / 2;

        DrawTriangle(
            Vector2{flag_x + flag_width, flag_y},
            Vector2{flag_x, flag_y + flag_height / 2},
            Vector2{flag_x + flag_width, flag_y + flag_height / 2},
            ToRaylibColor(MSColor(231, 76, 60))
        );

        DrawRectangle(flag_x, flag_y + flag_height / 2, 2, flag_height / 2,
                     ToRaylibColor(MSColor(44, 62, 80)));
    }
}

void Renderer::render_ui_panel(const GameEngine& engine) {
    int window_width = GetScreenWidth();

    DrawRectangle(0, 0, window_width, config_.ui_panel_height, ToRaylibColor(MSColor(52, 73, 94)));

    int minutes = static_cast<int>(engine.get_elapsed_time()) / 60;
    int seconds = static_cast<int>(engine.get_elapsed_time()) % 60;
    std::ostringstream time_stream;
    time_stream << std::setfill('0') << std::setw(2) << minutes << ":"
                << std::setfill('0') << std::setw(2) << seconds;

    DrawText(time_stream.str().c_str(), 20, 20, 30, ToRaylibColor(MSColor(236, 240, 241)));

    uint32_t mines_remaining = engine.get_mine_count() - engine.get_flags_placed();
    std::string mines_text = "Mines: " + std::to_string(mines_remaining);
    DrawText(mines_text.c_str(), 20, 50, 20, ToRaylibColor(MSColor(236, 240, 241)));

    const char* state_text = "";
    MSColor state_color = MSColor(236, 240, 241);

    switch (engine.get_state()) {
        case GameState::Ready:
            state_text = "Click to start";
            state_color = MSColor(52, 152, 219);
            break;
        case GameState::Playing:
            state_text = "Playing";
            state_color = MSColor(46, 204, 113);
            break;
        case GameState::Won:
            state_text = "You Won!";
            state_color = MSColor(39, 174, 96);
            break;
        case GameState::Lost:
            state_text = "Game Over";
            state_color = MSColor(231, 76, 60);
            break;
    }

    int state_width = MeasureText(state_text, 30);
    DrawText(state_text, window_width - state_width - 20, 20, 30, ToRaylibColor(state_color));

    std::string seed_text = "Seed: " + std::to_string(engine.get_seed());
    int seed_width = MeasureText(seed_text.c_str(), 16);
    DrawText(seed_text.c_str(), window_width - seed_width - 20, 55, 16,
            ToRaylibColor(MSColor(189, 195, 199)));
}

void Renderer::render_debug_overlay(const GameEngine& engine) {
    std::ostringstream debug_stream;
    debug_stream << "DEBUG MODE\n";
    debug_stream << "Board: " << engine.get_width() << "x" << engine.get_height() << "\n";
    debug_stream << "Mines: " << engine.get_mine_count() << "\n";
    debug_stream << "Revealed: " << engine.get_revealed_count() << "\n";
    debug_stream << "Flags: " << engine.get_flags_placed() << "\n";
    debug_stream << "Moves: " << engine.get_move_history().size() << "\n";
    debug_stream << "Seed: " << engine.get_seed();

    std::string debug_text = debug_stream.str();
    int line_height = 20;
    int y_offset = config_.ui_panel_height + 10;

    DrawRectangle(10, y_offset - 5, 250, 150, ToRaylibColor(MSColor(0, 0, 0, 180)));

    std::istringstream stream(debug_text);
    std::string line;
    while (std::getline(stream, line)) {
        DrawText(line.c_str(), 15, y_offset, 16, ToRaylibColor(MSColor(255, 255, 0)));
        y_offset += line_height;
    }
}

void Renderer::render_message() {
    if (message_timer_ > 0.0f) {
        message_timer_ -= GetFrameTime();

        int window_width = GetScreenWidth();
        int window_height = GetScreenHeight();

        int font_size = 30;
        int text_width = MeasureText(message_text_.c_str(), font_size);

        int box_padding = 20;
        int box_width = text_width + box_padding * 2;
        int box_height = font_size + box_padding * 2;

        int box_x = (window_width - box_width) / 2;
        int box_y = window_height - box_height - 50;

        float alpha = message_timer_ < 0.5f ? (message_timer_ / 0.5f) : 1.0f;

        DrawRectangle(box_x, box_y, box_width, box_height,
                     ToRaylibColor(MSColor(0, 0, 0, static_cast<unsigned char>(200 * alpha))));
        DrawText(message_text_.c_str(), box_x + box_padding, box_y + box_padding,
                font_size, ToRaylibColor(MSColor(255, 255, 255, static_cast<unsigned char>(255 * alpha))));
    }
}

bool Renderer::get_hovered_cell(const GameEngine& engine, uint32_t& out_x, uint32_t& out_y) {
    Vector2 mouse_pos = GetMousePosition();

    int32_t rel_x = static_cast<int32_t>(mouse_pos.x) - board_offset_x_;
    int32_t rel_y = static_cast<int32_t>(mouse_pos.y) - board_offset_y_;

    if (rel_x < 0 || rel_y < 0) return false;

    uint32_t cell_x = static_cast<uint32_t>(rel_x) / tile_size_;
    uint32_t cell_y = static_cast<uint32_t>(rel_y) / tile_size_;

    if (cell_x >= engine.get_width() || cell_y >= engine.get_height()) {
        return false;
    }

    out_x = cell_x;
    out_y = cell_y;
    return true;
}

void Renderer::handle_input(GameEngine& engine) {
    uint32_t cell_x, cell_y;
    if (get_hovered_cell(engine, cell_x, cell_y)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            engine.reveal(cell_x, cell_y);
        } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            engine.toggle_flag(cell_x, cell_y);
        }
    }

    if (IsKeyPressed(KEY_R)) {
        engine.reset();
        show_message("Game reset", 2.0f);
    }

    if (IsKeyPressed(KEY_D)) {
        toggle_debug_overlay();
    }

    if (IsKeyPressed(KEY_F)) {
        toggle_fps_counter();
    }

    if (IsKeyPressed(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) {
        if (engine.undo()) {
            show_message("Undo", 1.0f);
        }
    }

    if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL)) {
        if (engine.save_to_file("minesweeper_save.dat")) {
            show_message("Game saved", 2.0f);
        } else {
            show_message("Save failed", 2.0f);
        }
    }

    if (IsKeyPressed(KEY_L) && IsKeyDown(KEY_LEFT_CONTROL)) {
        if (engine.load_from_file("minesweeper_save.dat")) {
            show_message("Game loaded", 2.0f);
        } else {
            show_message("Load failed", 2.0f);
        }
    }

    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
        set_tile_size(tile_size_ + 4);
    }

    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        if (tile_size_ > 16) {
            set_tile_size(tile_size_ - 4);
        }
    }
}

void Renderer::show_message(const std::string& message, float duration) {
    message_text_ = message;
    message_duration_ = duration;
    message_timer_ = duration;
}

void Renderer::set_tile_size(uint32_t size) {
    tile_size_ = std::max(16u, std::min(64u, size));
}

} // namespace minesweeper