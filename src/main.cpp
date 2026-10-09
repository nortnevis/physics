#include "event_handler.hpp"
#include "particles.hpp"
#include "render.hpp"

using namespace ph;

int main(int, char **) {
    try {
        std::println("Config:\n\twidth: {}\n\theight: {}\n\tfps: {}\n\ttitle: {}", global::config.screen_width,
                     global::config.screen_height, global::config.fps, global::config.title);
        std::println("\taddr: {}", (void *)&global::config);
        InitWindow(global::config.screen_width, global::config.screen_height, global::config.title.c_str());
        SetTargetFPS(global::config.fps);

        init_particles();
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
