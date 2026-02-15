#include "core/Application.h"
#include <raylib.h>

namespace minesweeper {

Application::Application()
    : last_frame_time_(0.0f)
{
}

Application::~Application() {
    shutdown();
}

void Application::initialize() {
    GameConfig game_config;
    game_config.width = 16;
    game_config.height = 16;
    game_config.mine_count = 40;
    game_config.seed = 0;
    
    engine_ = std::make_unique<GameEngine>(game_config);
    
    RenderConfig render_config;
    render_config.window_width = 1024;
    render_config.window_height = 768;
    render_config.tile_size = 32;
    render_config.ui_panel_height = 80;
    render_config.vsync = true;
    render_config.title = "Minesweeper - Advanced Edition";
    
    renderer_ = std::make_unique<Renderer>(render_config);
    
    last_frame_time_ = 0.0f;
}

void Application::run() {
    initialize();
    
    while (!renderer_->should_close()) {
        update();
        render();
    }
}

void Application::update() {
    float current_time = GetTime();
    float delta_time = current_time - last_frame_time_;
    last_frame_time_ = current_time;
    
    engine_->update(delta_time);
    renderer_->handle_input(*engine_);
}

void Application::render() {
    renderer_->begin_frame();
    renderer_->render(*engine_);
    renderer_->end_frame();
}

void Application::shutdown() {
    renderer_.reset();
    engine_.reset();
}

} // namespace minesweeper
