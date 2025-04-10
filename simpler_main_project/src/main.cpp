#include "bt.hpp"
#include "battery.hpp"
#include "buttons.hpp"
#include "shutdown.hpp"

#include <hardware/clocks.h>
#include <hardware/rosc.h>
#include <hardware/structs/rosc.h>
#include <hardware/uart.h>
#include <hardware/watchdog.h>
#include <hci.h>
#include <pico/cyw43_arch.h>
#include <pico/runtime_init.h>
#include <pico/sleep.h>
#include <pico/stdlib.h>

int main()
{
    stdio_init_all();

    cyw43_arch_init();

    printf("Hello world!\n");

    bt::init();
    buttons::init();
    shutdown::init();
    battery::init();

    printf("Starting loop\n");
    btstack_run_loop_execute();

    printf("BTStack loop done.\n");
    cyw43_arch_deinit();
    printf("CYW43 deinitialized\n");

    printf("HCI shutdown\n");
    uart_default_tx_wait_blocking();

    uint scb_orig    = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

    gpio_pull_up(2);
    sleep_run_from_xosc();

    sleep_goto_dormant_until_edge_high(2);

    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_LSB);

    scb_hw->scr          = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    /*rosc_enable();*/
    clocks_init();
    stdio_init_all();

    gpio_set_pulls(2, false, false);

    /*printf("Woke from sleep, time to restart!\n");*/
    /*uart_default_tx_wait_blocking();*/

    watchdog_enable(100, true);
    while (true)
        ;
}
