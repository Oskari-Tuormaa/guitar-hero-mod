#include "shutdown.hpp"
#include "topics.hpp"

#include <btstack.h>
#include <cstdint>
#include <cstdio>
#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <pico/sleep.h>

#include "timer.hpp"

namespace config
{

    constexpr uint32_t shutdown_time_ms = 10'000;
    constexpr uint32_t reboot_gpio      = 15;

} // namespace config

namespace globals
{

    static OneShotTimer shutdown_timer;

} // namespace globals

void reboot()
{
    watchdog_enable(100, true);
    while (true)
        ;
}

void init_shutdown()
{
    globals::shutdown_timer.set_timeout(config::shutdown_time_ms);
    globals::shutdown_timer.set_callback([] {
        printf("Going into dormant mode...\n");
        gpio_set_pulls(config::reboot_gpio, true, false);
        sleep_goto_dormant_until_edge_high(config::reboot_gpio);
        reboot();
    });

    Topic::ButtonStateChange::subscribe(
        [](auto) { globals::shutdown_timer.restart(); });

    printf("Starting shutdown timer for %d ms\n", config::shutdown_time_ms);
    globals::shutdown_timer.start();
}
