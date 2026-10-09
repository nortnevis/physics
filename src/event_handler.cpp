#include "event_handler.hpp"
#include "particles.hpp"

namespace ph {

void event_handling_loop(std::stop_token token) noexcept {
    auto &grid_param = global::config.grid_params;
    auto &grid = global::particles_grid;
    auto &templates = global::particles_templates;
    auto &data = global::particles_data;
    while (!token.stop_requested()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto pos = GetMousePosition();
            if (pos.x < 0 || pos.y < 0) {
                continue;
            }
            if (pos.x < grid_param.width && pos.y < grid_param.height) {
                constexpr auto dust = ParticleType::DUST;
                auto offset = Position::as_offset((int)pos.x, (int)pos.y);
                grid[offset] = dust;
                data[offset] = templates[(int)dust];
            }
        }
    }
    std::println("Stop event handling.");
}

} // namespace ph
