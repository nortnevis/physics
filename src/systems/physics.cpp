#include "physics.hpp"

namespace ph {

void gravity_system(std::stop_token token) noexcept {
    // auto &grid_param = global::config.grid_params;
    // auto &grid = global::particles_grid;
    // auto &templates = global::particles_templates;
    // auto &data = global::particles_data;

    while (!token.stop_requested()) {
        // for (int y = 0; y < grid_params.height; ++y) {
        //     for (int x = 0; x < grid_params.width; ++x) {
        //         // auto particle = data[Position::as_offset(x, y)];
        //         // DrawRectangle(x, y, grid_params.particle_width, grid_params.particle_height, particle.color);
        //     }
        // }
    }
}

} // namespace ph
