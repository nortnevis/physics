#include "particles.hpp"
#include "systems/event_handler.hpp"
#include "systems/physics.hpp"
#include "systems/render.hpp"

using namespace ph;

int main(int, char **) {
    try {
        auto &conf = global::config;
        conf.screen_width = 800;
        conf.screen_height = 600;
        conf.grid_params.width = conf.screen_width;
        conf.grid_params.height = conf.screen_height;
        conf.grid_params.total = conf.screen_width * conf.screen_height;
        std::println("Config:\n\twidth: {}\n\theight: {}\n\tfps: {}\n\ttitle: {}", conf.screen_width,
                     conf.screen_height, conf.fps, conf.title);
        std::println("\taddr: {}", (void *)&conf);
        InitWindow(conf.screen_width, conf.screen_height, conf.title.c_str());
        SetTargetFPS(conf.fps);

        init_particles();
        {
            auto event_loop = std::jthread(event_handling_loop);
            auto physics_loop = std::jthread(gravity_system);
            render_loop();
        } // all jtrhead will join threre
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
