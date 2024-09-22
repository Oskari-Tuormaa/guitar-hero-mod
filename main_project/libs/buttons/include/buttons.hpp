#pragma once

#include <pico/stdlib.h>

void register_button_state_listener(uint gpio);

template<typename T>
void register_button_state_listeners(const T &gpios) {
    for (uint gpio : gpios) {
        register_button_state_listener(gpio);
    }
}
