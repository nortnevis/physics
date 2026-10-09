#include "particles.hpp"
#include "render.hpp"
#include <rlgl.h>

namespace ph {

namespace cpu {

void draw_particles() noexcept {
    auto &grid_params = global::config.grid_params;
    auto &templates = global::particles_templates;
    auto &data = global::particles_data;

    static const auto void_color = templates[(int)ParticleType::VOID].color;
    DrawRectangle(0, 0, grid_params.width, grid_params.height, void_color);

    for (int y = 0; y < grid_params.height; ++y) {
        for (int x = 0; x < grid_params.width; ++x) {
            auto particle = data[Position::as_offset(x, y)];
            DrawRectangle(x, y, grid_params.particle_width, grid_params.particle_height, particle.color);
        }
    }
}

} // namespace cpu

namespace gpu {

void draw_particles() noexcept { assert(false && "Not implemented yet!"); }

} // namespace gpu

void render_loop() noexcept {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
        if (global::config.mode == Mode::GPU) {
            gpu::draw_particles();
        } else {
            cpu::draw_particles();
        }

        ManualDrawFPS();
        ManualEndDrawing(); // apply fix for EndDrawing issue for vcpkg 6.0 version
    }
}

static int64_t current_fps = 0;

void ManualEndDrawing() noexcept {
    static double previous_time = GetTime();
    double target_time = 1.0 / global::config.fps;

    // flush the render batch data manually
    rlDrawRenderBatchActive();

    // swap the back and front buffers to display the frame
    SwapScreenBuffer();

    // handle frame rate limits
    auto delta = GetTime() - previous_time;
    if (delta < target_time) {
        WaitTime(target_time - delta);
    }
    current_fps = std::lround(1.0 / (GetTime() - previous_time));
    previous_time = GetTime();

    PollInputEvents();
}

void ManualDrawFPS() noexcept {
    DrawText(TextFormat("%i FPS", current_fps), 10, 10, 20, LIME);
    DrawText("vcpkg 6.0 EndDrawing patch active", 10, 40, 20, DARKBLUE);
}

} // namespace ph
