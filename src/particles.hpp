#pragma once

namespace ph {

enum class ParticleType : int {
    VOID = 0,
    DUST,
    _count,
};

std::expected<size_t, std::errc> as_offset(int x, int y) noexcept;
std::expected<size_t, std::errc> as_offset(const Vector2 &pos) noexcept;

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

struct Buffer {
    // TODO: Requirements:
    // 1. each node contains vector with particles
    // 2. each node will have atomic flag, signalling about modification of particles
    // 3. each node has counter for particles
    // 4. no lock, only swapping double buffers for modification (delete, editing)
    struct Node {};
};

namespace global {

inline std::vector<Particle> particles_templates{};
inline std::vector<Particle> particles_data{};

} // namespace global

Color get_particle_default_color(ParticleType type) noexcept;

void init_particles();

} // namespace ph
