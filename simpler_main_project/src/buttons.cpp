#include "buttons.hpp"

#include "bt.hpp"
#include "hid.hpp"
#include "shutdown.hpp"
#include "utils.hpp"

#include <array>
#include <btstack.h>
#include <cstdint>
#include <hardware/gpio.h>
#include <pico/time.h>
#include <pico/types.h>

namespace buttons
{

namespace
{

constexpr auto button_gpios = std::to_array<uint8_t>({
    1,
    2,
    3,
    4,
    10,
    11,
    12,
    13,
    14,
});

constexpr int64_t      debounce_time_ms = T_MS(1);
btstack_timer_source_t debounce_timer;

std::array<bool, button_gpios.size()> button_states {};
absolute_time_t                       last_debounce_time = 0;

void debounce_timer_handler(btstack_timer_source_t* ts)
{
    UNUSED(ts);

    auto* report = hid::get_report();

    for (size_t i = 0; i < button_gpios.size(); i++)
    {
        auto  gpio      = button_gpios[i];
        auto& state     = button_states[i];
        auto  new_state = !gpio_get(gpio);

        if (state != new_state)
        {
            state = new_state;

            if (state)
                report->buttons |= BIT(i);
            else
                report->buttons &= IBIT(i);

            bt::send_hid_report();
            shutdown::restart();
        }
    }

    btstack_run_loop_set_timer(&debounce_timer, debounce_time_ms);
    btstack_run_loop_add_timer(&debounce_timer);
}

// NOLINTNEXTLINE [bugprone-easily-swappable-parameters]
void gpio_callback(uint gpio, uint32_t events)
{
    UNUSED(gpio);
    UNUSED(events);

    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_debounce_time, now) < T_MS_TO_US(debounce_time_ms))
        return;

    last_debounce_time = now;
}

} // namespace

void init()
{
    // Initialize GPIOs
    for (auto gpio : button_gpios)
    {
        gpio_init(gpio);
        gpio_pull_up(gpio);
        gpio_set_dir(gpio, GPIO_IN);
        // gpio_set_irq_enabled_with_callback(gpio,
        //                                    GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        //                                    true,
        //                                    &gpio_callback);
    }

    btstack_run_loop_set_timer_handler(&debounce_timer, debounce_timer_handler);
    btstack_run_loop_set_timer(&debounce_timer, debounce_time_ms);
    btstack_run_loop_add_timer(&debounce_timer);
}

} // namespace buttons
