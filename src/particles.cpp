#include "particles.hpp"

namespace ph {

size_t as_offset(float x, float y) noexcept {
    int mul = (int)x * (int)y;
    assert(mul < global::config.grid_params.total && mul > 0);
    return (int)y * global::config.grid_params.width + (int)x;
}

size_t as_offset(const Vector2 &pos) noexcept { return as_offset(pos.x, pos.y); }

Color get_particle_default_color(ParticleType type) noexcept { return global::particles_templates[(int)type].color; }

void init_particles() {
    test_assertions();
    global::particles_data.reserve(global::config.grid_params.total);

    global::particles_templates = std::vector<Particle>((int)ParticleType::_count);
    auto &templates = global::particles_templates;
    templates[(int)ParticleType::VOID] = {};
    templates[(int)ParticleType::DUST] = {
        .type = ParticleType::DUST,
        .color = YELLOW,
        .density = 1500.f,
        .mass = 0.001f,
        .velocity = {0.f, 0.f},
    };
}

} // namespace ph
