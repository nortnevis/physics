#pragma once

#include "utils.hpp"

namespace ph {

struct Config {
  private:
    struct GridParams {
        int width = 600;
        int height = 450;
        int total = width * height;
        int particle_width = 1;
        int particle_height = 1;
    };

  public:
    std::string title = "Physics";
    int screen_width = 800;
    int screen_height = 600;
    int fps = 60;
    Mode mode = Mode::CPU;
    GridParams grid_params;
};

namespace global {

inline Config config{};

}

inline void test_assertions() {
    auto &grid = global::config.grid_params;
    assert(grid.width * grid.height == grid.total);
    assert(grid.width <= global::config.screen_width);
    assert(grid.height <= global::config.screen_height);
}

} // namespace ph
