#include "buttons.hpp"
#include "topics.hpp"

#include <array>

static std::array<uint, 32> integrators;

void gpio_interrupt_callback(uint pin, uint32_t mask)
{
    auto state = gpio_get(pin)
                   ? Topic::ButtonStateChange::ButtonState::RELEASED
                   : Topic::ButtonStateChange::ButtonState::PRESSED;

    Topic::ButtonStateChange::publish(
        { .new_state = state, .button_gpio = pin });
}

void register_button_state_listener(uint gpio)
{
    gpio_set_pulls(gpio, true, false);
    gpio_set_irq_enabled_with_callback(gpio,
                                       GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                                       true, gpio_interrupt_callback);
}
