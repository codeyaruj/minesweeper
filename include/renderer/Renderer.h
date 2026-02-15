#pragma once

#include "engine/GameEngine.h"
#include <memory>
#include <string>

namespace minesweeper {

struct RenderConfig {
    uint32_t window_width = 1024;
    uint32_t window_height = 768;
    uint32_t tile_size = 32;
    uint32_t ui_panel_height = 80;
    bool vsync = true;
    std::string title = "Minesweeper";
};

struct MSColor {
    unsigned char r, g, b, a;
    MSColor(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}
};

class Renderer {
public:
    explicit Renderer(const RenderConfig& config);
    ~Renderer();
    
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    
    bool should_close() const;
    void begin_frame();
    void end_frame();
    
    void render(const GameEngine& engine);
    
    void handle_input(GameEngine& engine);
    
    void toggle_debug_overlay() { show_debug_overlay_ = !show_debug_overlay_; }
    void toggle_fps_counter() { show_fps_ = !show_fps_; }
    
    void show_message(const std::string& message, float duration);
    
    uint32_t get_tile_size() const { return tile_size_; }
    void set_tile_size(uint32_t size);

private:
    void render_board(const GameEngine& engine);
    void render_cell(const Cell& cell, uint32_t x, uint32_t y, bool hovered, bool debug_mode);
    void render_ui_panel(const GameEngine& engine);
    void render_debug_overlay(const GameEngine& engine);
    void render_message();
    
    void calculate_board_position(const GameEngine& engine);
    bool get_hovered_cell(const GameEngine& engine, uint32_t& out_x, uint32_t& out_y);
    
    MSColor get_number_color(uint8_t count);

private:
    RenderConfig config_;
    uint32_t tile_size_;
    int32_t board_offset_x_;
    int32_t board_offset_y_;
    
    bool show_debug_overlay_;
    bool show_fps_;
    
    std::string message_text_;
    float message_timer_;
    float message_duration_;
    
    static constexpr float CELL_PADDING = 2.0f;
};

} // namespace minesweeper