#include <cstdio>
#include <pico/stdlib.h>

#include <pico/sleep.h>

#include <hardware/gpio.h>
#include <hardware/watchdog.h>

void reboot()
{
    watchdog_enable(100, true);
    while (true)
        ;
}

int main()
{
    stdio_init_all();
    printf("Hello!\r\n");

    printf("Going to sleep...\n");
    gpio_set_pulls(15, true, false);
    sleep_goto_dormant_until_edge_high(15);
    printf("Woke from dormant!\n");

    sleep_ms(4000);

    printf("Rebooting...\n");
    reboot();
}
