#include "../particles.hpp"
#include "event_handler.hpp"

namespace ph {

void event_handling_loop(std::stop_token token) noexcept {
    auto &grid_param = global::config.grid_params;
    auto &templates = global::particles_templates;
    auto &data = global::particles_data;

    static double previous_time = GetTime();

    Vector2 prev{0.f, 0.f};
    while (!token.stop_requested()) {
        PollInputEvents();

        if (bool alt = IsKeyDown(KEY_LEFT_ALT); IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto pos = GetMousePosition();
            if (pos.x < 0 || pos.y < 0 || (pos.x == prev.x && pos.y == prev.y)) {
                continue;
            }
            if (pos.x < grid_param.width && pos.y < grid_param.height) {
                // TODO: add mouse scrolling to change magnifier
                int magnifier = 1000;
                for (int i = 0; i < 1 + (magnifier - 1) * alt; ++i) {
                    auto dust = templates[(int)ParticleType::DUST];
                    dust.id = data.size();
                    dust.pos = pos;
                    if (alt) {
                        dust.pos.x += rand_float(-5.f, 5.f);
                        dust.pos.y += rand_float(-5.f, 5.f);
                    }
                    data.push_back(std::move(dust));
                }
            }
            prev = pos;
        }

        auto delta = GetTime() - previous_time;
        if (delta < global::config.target_frame_time) {
            WaitTime(global::config.target_frame_time - delta);
        }
        previous_time = GetTime();
    }
    std::println("Stop event handling.");
}

} // namespace ph
