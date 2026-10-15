#pragma once

namespace ph {

namespace cpu {
void draw_particles() noexcept;
}

namespace gpu {
void draw_particles() noexcept;
}

void render_loop() noexcept;

void ManualEndDrawing() noexcept;

void ManualDrawFPS() noexcept;

} // namespace ph
