#include "event_handler.hpp"

namespace ph {

void event_handling_loop(std::stop_token token) noexcept {
    while (!token.stop_requested()) {
        if (bool is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT); is_pressed) {
            std::println("LBM: {}", is_pressed);
        }
    }
    std::println("Stop event handling.");
}

} // namespace ph
