#include "particles.hpp"

namespace ph {

std::expected<size_t, std::errc> as_offset(int x, int y) noexcept {
    if (x >= global::config.grid_params.width || x < 0 || y >= global::config.grid_params.height || y < 0)
        [[unlikely]] {
        return std::unexpected(std::errc::result_out_of_range);
    }
    return (int)y * global::config.grid_params.width + (int)x;
}

std::expected<size_t, std::errc> as_offset(const Vector2 &pos) noexcept { return as_offset((int)pos.x, (int)pos.y); }

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
