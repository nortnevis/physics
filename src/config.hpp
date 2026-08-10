#pragma once

namespace ph {

struct Config {
    std::string title = "Physics";
    int screen_width = 800;
    int screen_height = 600;
    int fps = 60;
    Mode mode = Mode::GPU;
};

extern Config config;

} // namespace ph
