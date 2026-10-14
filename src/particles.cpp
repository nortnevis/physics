#include "particles.hpp"

namespace ph {

size_t Position::as_offset() noexcept { return y * global::config.grid_params.width + x; }

size_t Position::as_offset(int x, int y) noexcept { return y * global::config.grid_params.width + x; }

size_t Position::as_offset(const Position &pos) noexcept { return pos.y * global::config.grid_params.width + pos.x; }

Position Position::from_vector2(const Vector2 pos) noexcept { return {.x = (int)pos.x, .y = (int)pos.y}; }

Vector2 Position::to_vector2(const Position &pos) noexcept { return {.x = (float)pos.x, .y = (float)pos.y}; }

Color get_particle_default_color(ParticleType type) noexcept { return global::particles_templates[(int)type].color; }

void init_particles() {
    test_assertions();
    global::particles_grid = std::vector<ParticleType>(global::config.grid_params.total);
    global::particles_data = std::vector<Particle>(global::config.grid_params.total);

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

void physics_loop() noexcept {
    // hello
    // world
}

} // namespace ph
