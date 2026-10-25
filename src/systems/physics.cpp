#include "../particles.hpp"
#include "physics.hpp"

namespace ph {

constexpr Vector2 _g = {0.f, 9.81f};
// static auto &scale = global::config.pixel_scale_factor;
// Vector2 _g_scaled = {_g.x * scale, _g.y * scale};

void gravity_system(std::stop_token token) noexcept {
    auto &data = global::particles_data;

    static double prev_time = GetTime();
    while (!token.stop_requested()) {
        // TODO: Make target time a global config
        double target_time = 1.0 / global::config.fps;
        auto delta = GetTime() - prev_time;
        if (delta < target_time) {
            WaitTime(target_time - delta);
            delta = GetTime() - prev_time;
        }
        prev_time = GetTime();
        float dt = (float)delta;
        // TODO: Fix data race
        for (auto &p : data) {
            auto &vx = p.velocity.x;
            auto &vy = p.velocity.y;
            vx += _g.x * dt;
            vy += _g.y * dt;
            // TODO: check for boundaries
            p.pos.x += vx * dt;
            p.pos.y += vy * dt;
        }
    }
}

} // namespace ph
