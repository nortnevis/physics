#include "event_handler.hpp"
#include "particles.hpp"

namespace ph {

void event_handling_loop(std::stop_token token) noexcept {
    auto &grid_param = global::config.grid_params;
    while (!token.stop_requested()) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto pos = GetMousePosition();
            if (pos.x < 0 || pos.y < 0) {
                continue;
            }
            if (pos.x <= grid_param.width && pos.y <= grid_param.height) {
                global::particles_grid[Position::as_offset((int)pos.x, (int)pos.y)] = ParticleType::DUST;
            }
        }
    }
    std::println("Stop event handling.");
}

} // namespace ph
