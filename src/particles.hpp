#pragma once

namespace ph {

enum class ParticleType : int {
    VOID = 0,
    DUST,
    _count,
};

struct Position {
    int x = 0;
    int y = 0;

    size_t as_offset() { return y * global::config.grid_params.width + x; }
    static size_t as_offset(int x, int y) { return y * global::config.grid_params.width + x; }
    static size_t as_offset(const Position &pos) { return pos.y * global::config.grid_params.width + pos.x; }
    static Position from_vector2(const Vector2 pos) { return {.x = (int)pos.x, .y = (int)pos.y}; }
    static Vector2 to_vector2(const Position &pos) { return {.x = (float)pos.x, .y = (float)pos.y}; }
};

struct Particle {
    ParticleType type = ParticleType::VOID;
    Color color = BLACK;
    float density = 0;
    float mass = 0;
    Vector2 velocity = {0.f, 0.f};
};

namespace global {

inline std::vector<Particle> particles_templates{};
inline std::vector<Particle> particles_data{};

inline std::vector<ParticleType> particles_grid{};

} // namespace global

inline void init_particles() {
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

} // namespace ph
