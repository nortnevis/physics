#include "particles.hpp"

namespace ph {

size_t Position::as_offset() noexcept { return y * global::config.grid_params.width + x; }

size_t Position::as_offset(int x, int y) noexcept { return y * global::config.grid_params.width + x; }

size_t Position::as_offset(const Position &pos) noexcept { return pos.y * global::config.grid_params.width + pos.x; }

Position Position::from_vector2(const Vector2 pos) noexcept { return {.x = (int)pos.x, .y = (int)pos.y}; }

Vector2 Position::to_vector2(const Position &pos) noexcept { return {.x = (float)pos.x, .y = (float)pos.y}; }

} // namespace ph
