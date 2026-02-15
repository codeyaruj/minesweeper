#pragma once

#include "engine/GameEngine.h"
#include "renderer/Renderer.h"
#include <memory>

namespace minesweeper {

class Application {
public:
    Application();
    ~Application();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void run();
    
private:
    void initialize();
    void update();
    void render();
    void shutdown();

private:
    std::unique_ptr<GameEngine> engine_;
    std::unique_ptr<Renderer> renderer_;
    
    float last_frame_time_;
};

} // namespace minesweeper
