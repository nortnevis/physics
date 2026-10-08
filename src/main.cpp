#include "config.hpp"
#include "event_handler.hpp"
#include "render.hpp"

using namespace ph;

int main(int, char **) {
    std::println("Config:\n\twidth: {}\n\theight: {}\n\tfps: {}\n\ttitle: {}", global_config.screen_width,
                 global_config.screen_height, global_config.fps, global_config.title);
    std::println("\taddr: {}", (void *)&global_config);
    InitWindow(global_config.screen_width, global_config.screen_height, global_config.title.c_str());
    SetTargetFPS(global_config.fps);

    auto event_loop = std::jthread(event_handling_loop);
    render_loop();
    CloseWindow();
    return 0;
}
