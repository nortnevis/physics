#include "config.hpp"
#include "render.hpp"
#include <rlgl.h>

namespace ph {

static int64_t current_fps = 0;

void ManualEndDrawing() {
    static double previous_time = GetTime();
    double target_time = 1.0 / global_config.fps;

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

void ManualDrawFPS() {
    DrawText(TextFormat("%i FPS", current_fps), 10, 10, 20, LIME);
    DrawText("vcpkg 6.0 EndDrawing patch active", 10, 40, 20, DARKBLUE);
}

void render_loop() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);

        ManualDrawFPS();
        ManualEndDrawing(); // apply fix for EndDrawing issue for vcpkg 6.0 version
    }
}

} // namespace ph
