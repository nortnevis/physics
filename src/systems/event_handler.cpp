#include "../particles.hpp"
#include "event_handler.hpp"

namespace ph {

void event_handling_loop(std::stop_token token) noexcept {
    auto &grid_param = global::config.grid_params;
    auto &templates = global::particles_templates;
    auto &data = global::particles_data;

    static double previous_time = GetTime();
    double target_time = 1.0 / global::config.fps;

    while (!token.stop_requested()) {
        PollInputEvents();

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            auto pos = GetMousePosition();
            if (pos.x < 0 || pos.y < 0) {
                continue;
            }
            if (pos.x < grid_param.width && pos.y < grid_param.height) {
                auto dust = templates[(int)ParticleType::DUST];
                dust.id = data.size();
                dust.pos = pos;
                data.push_back(std::move(dust));
            }
        }

        auto delta = GetTime() - previous_time;
        if (delta < target_time) {
            WaitTime(target_time - delta);
        }
        previous_time = GetTime();
    }
    std::println("Stop event handling.");
}

} // namespace ph
