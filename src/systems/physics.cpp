#include "../particles.hpp"
#include "physics.hpp"

namespace ph {

constexpr Vector2 _g = {0.f, 9.81f};
Vector2 _g_scaled = Vector2Scale(_g, global::config.pixel_scale_factor);
auto &conf = global::config;

void gravity_system(std::stop_token token) noexcept {
    auto &data = global::particles_data;

    static double prev_time = GetTime();
    while (!token.stop_requested()) {
        auto delta = GetTime() - prev_time;
        if (delta < conf.target_frame_time) {
            WaitTime(conf.target_frame_time - delta);
            delta = GetTime() - prev_time;
        }
        prev_time = GetTime();
        float dt = (float)delta;
        // TODO: add physics of density and overlaying paritles on each other
        // TODO: dissipate kinetic energy through heat
        // TODO: add option to absorb energy from particles to borders
        // TODO: add option to remove borders
        // TODO: add option to loop topology
        // TODO: Fix data race
        for (auto &p : data) {
            // Update velocity
            // TODO: move this block of code to another function like apply_force
            auto dv = Vector2Scale(_g_scaled, dt);
            p.velocity = Vector2Add(p.velocity, dv);

            // Update position
            auto delta_pos = Vector2Scale(p.velocity, dt);
            auto new_pos = Vector2Add(p.pos, delta_pos);
            // TODO: check if we can optimize it by creating loops for every condition block
            if (new_pos.y >= conf.grid_params.height) {
                p.velocity.y *= -1.f;
                delta_pos.y -= new_pos.y - conf.grid_params.height;
            }
            if (new_pos.y < 0) {
                p.velocity.y *= -1.f;
                delta_pos.y -= new_pos.y;
            }

            if (new_pos.x >= conf.grid_params.width) {
                p.velocity.x *= -1.f;
                delta_pos.x -= new_pos.x - conf.grid_params.width;
            }
            if (new_pos.x < 0) {
                p.velocity.x *= -1.f;
                delta_pos.x -= new_pos.x;
            }

            // Commit new position
            p.pos = Vector2Add(p.pos, delta_pos);
        }
    }
}

} // namespace ph
