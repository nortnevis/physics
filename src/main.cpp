#include "config.hpp"
#include "event_handler.hpp"
#include "render.hpp"

using namespace ph;

int main(int, char **) {
    try {
        std::println("Config:\n\twidth: {}\n\theight: {}\n\tfps: {}\n\ttitle: {}", global_config.screen_width,
                     global_config.screen_height, global_config.fps, global_config.title);
        std::println("\taddr: {}", (void *)&global_config);
        InitWindow(global_config.screen_width, global_config.screen_height, global_config.title.c_str());
        SetTargetFPS(global_config.fps);

        auto event_loop = std::jthread(event_handling_loop);
        render_loop();
        CloseWindow();
    } catch (const std::system_error &err) {
        std::println("System error {}[{}]: {}.", err.code().value(), err.code().message(), err.what());
    } catch (const std::exception &e) {
        std::println("Exception: {}.", e.what());
    } catch (...) {
        std::println("Unknown exception.");
    }

    return 0;
}
