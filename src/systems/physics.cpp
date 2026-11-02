#include "../particles.hpp"
#include "physics.hpp"

namespace ph {

constexpr Vector2 _g = {0.f, 9.81f};
Vector2 _g_scaled = Vector2Scale(_g, global::config.pixel_scale_factor);

void gravity_system(std::stop_token token) noexcept {
    auto &data = global::particles_data;

    static double prev_time = GetTime();
    while (!token.stop_requested()) {
        auto delta = GetTime() - prev_time;
        if (delta < global::config.target_frame_time) {
            WaitTime(global::config.target_frame_time - delta);
            delta = GetTime() - prev_time;
        }
        prev_time = GetTime();
        float dt = (float)delta;
        // TODO: Fix data race
        for (auto &p : data) {
            auto dv = Vector2Scale(_g_scaled, dt);
            p.velocity = Vector2Add(p.velocity, dv);
            auto delta_pos = Vector2Scale(p.velocity, dt);
            // TODO: check for boundaries
            p.pos = Vector2Add(p.pos, delta_pos);
        }
    }
}

} // namespace ph
