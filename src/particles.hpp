#pragma once

namespace ph {

enum class ParticleType : int {
    VOID = 0,
    DUST,
    _count,
};

size_t as_offset(float x, float y) noexcept;
size_t as_offset(const Vector2 &pos) noexcept;

// Entity with components
struct Particle {
    int id = 0;
    ParticleType type = ParticleType::VOID;
    Vector2 pos = {0, 0};
    Color color = BLACK;
    float density = 0;
    float mass = 0;
    Vector2 velocity = {0.f, 0.f};
};

namespace global {

inline std::vector<Particle> particles_templates{};
inline std::vector<Particle> particles_data{};

} // namespace global

Color get_particle_default_color(ParticleType type) noexcept;

void init_particles();

} // namespace ph
