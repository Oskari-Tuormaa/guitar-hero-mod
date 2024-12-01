#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include <btstack_run_loop.h>

int btstack_main(void);
int main() {
    stdio_init_all();

    cyw43_arch_init();

    printf("Hello world!\n");

    btstack_main();

    btstack_run_loop_execute();
}
