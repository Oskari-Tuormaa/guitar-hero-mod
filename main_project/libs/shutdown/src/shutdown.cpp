#include "shutdown.hpp"
#include "topics.hpp"

#include <btstack.h>
#include <cstdint>
#include <cstdio>
#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include <pico/sleep.h>

#include <pico/runtime_init.h>

#include "timer.hpp"

#include <hardware/rosc.h>
#include <hardware/structs/rosc.h>
#include <pico/runtime_init.h>

namespace config
{

    constexpr uint32_t shutdown_time_ms = 5'000;
    constexpr uint32_t reboot_gpio      = 14;

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

void rosc_enable()
{
    uint32_t tmp = rosc_hw->ctrl;
    tmp &= (~ROSC_CTRL_ENABLE_BITS);
    tmp |= (ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB);
    rosc_write(&rosc_hw->ctrl, tmp);
    // Wait for stable
    while ((rosc_hw->status & ROSC_STATUS_STABLE_BITS) != ROSC_STATUS_STABLE_BITS);
}


void init_shutdown()
{
    globals::shutdown_timer.set_timeout(config::shutdown_time_ms);
    globals::shutdown_timer.set_callback([] {
        /*printf("Going into dormant mode...\n");*/
        btstack_run_loop_trigger_exit();
        gpio_set_pulls(config::reboot_gpio, true, false);
        /*sleep_run_from_rosc();*/
        sleep_goto_dormant_until_edge_high(config::reboot_gpio);
        clocks_init();
        reboot();
    });

    Topic::ButtonStateChange::subscribe(
        [](auto) { globals::shutdown_timer.restart(); });

    printf("Starting shutdown timer for %d ms\n", config::shutdown_time_ms);
    globals::shutdown_timer.start();
}
