#include "config.hpp"

using namespace ph;

int main(int, char **) {
    std::println("Hello, World!");
    std::println("Config:\n\twidth: {}\n\theight: {}\n\tfps: {}\n\ttitle: {}", config.screen_width,
                 config.screen_height, config.fps, config.title);
    std::println("\taddr: {}", (void *)&config);
    InitWindow(config.screen_width, config.screen_height, config.title.c_str());
    SetTargetFPS(config.fps);

    // main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();

        // TODO: Remove those lines after updating to raylib version >6.0
        PollInputEvents();
        SwapScreenBuffer();
    }
    CloseWindow();
    return 0;
}
