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

    size_t as_offset() noexcept;
};

size_t as_offset(int x, int y) noexcept;
size_t as_offset(const Position &pos) noexcept;
Position from_vector2(const Vector2 pos) noexcept;
Vector2 to_vector2(const Position &pos) noexcept;

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

Color get_particle_default_color(ParticleType type) noexcept;

void init_particles();

} // namespace ph
