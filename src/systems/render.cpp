#include "../particles.hpp"
#include "render.hpp"
#include <rlgl.h>

namespace ph {

static Image canvas_image{};
static Texture2D canvas_texture{};

namespace cpu {

void draw_particles() noexcept {
    static auto &grid_params = global::config.grid_params;
    static auto &data = global::particles_data;

    // TODO: maybe make a shader?
    auto *pixels = (unsigned char *)canvas_image.data;
    for (int y = 0; y < grid_params.height; ++y) {
        for (int x = 0; x < grid_params.width; ++x) {
            size_t offset = as_offset(x, y);
            size_t pixels_offset = offset * sizeof(Color);
            if (pixels_offset > global::particles_data.size() * sizeof(decltype(global::particles_data)::value_type)) {
                std::println("BAD! {} <- {}", pixels_offset, offset);
            }
            auto color = data[offset].color;
            pixels[pixels_offset + 0] = color.r;
            pixels[pixels_offset + 1] = color.g;
            pixels[pixels_offset + 2] = color.b;
            pixels[pixels_offset + 3] = color.a;
        }
    }
    UpdateTexture(canvas_texture, pixels);
    DrawTexture(canvas_texture, 0, 0, WHITE);
}

} // namespace cpu

namespace gpu {

void draw_particles() noexcept { assert(false && "Not implemented yet!"); }

} // namespace gpu

void render_loop() noexcept {
    Color fill_color = get_particle_default_color(ParticleType::VOID);

    canvas_image = {
        .data = new Color[global::config.grid_params.total]{fill_color},
        .width = global::config.grid_params.width,
        .height = global::config.grid_params.height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };

    canvas_texture = LoadTextureFromImage(canvas_image);

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
