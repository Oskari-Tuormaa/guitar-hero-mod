#include <btstack.h>

#include <cstring>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>

#include "bt.hpp"
#include "hid_stuff.h"
#include "timer.hpp"

enum class DummyOutputState
{
    PRESS_START,
    PRESS_DOWN,
    PRESS_UP,
};

void dummy_report_handler();
void dummy_start_stop_handler();

namespace config
{
    constexpr uint32_t dummy_report_timeout_ms = 1;

    constexpr uint32_t dummy_start_timeout_ms = 2'000;
    constexpr uint32_t dummy_stop_timeout_ms  = 20'000;
} // namespace config

namespace globals
{
    static bool dummy_running      = false;
    static int  dummy_output_state = 0;

    static report_t report;

    namespace timers
    {
        static OneShotTimer dummy_report_timer {
            dummy_report_handler, config::dummy_report_timeout_ms
        };
        static OneShotTimer dummy_start_stop_timer {
            dummy_start_stop_handler, config::dummy_start_timeout_ms
        };
    } // namespace timers
} // namespace globals

void dummy_report_handler()
{
    if (is_connected())
    {
#if 1
        if (globals::dummy_running)
        {
            if (globals::dummy_output_state % 10 == 5)
            {
                globals::report.hat = HatSwitchDirections::HATSWITCH_DOWN;
            }
            else if ((globals::dummy_output_state % 10) == 6)
            {
                globals::report.hat = HatSwitchDirections::HATSWITCH_NONE;
            }

            switch ((globals::dummy_output_state / 10) % 5)
            {
            case 0:
                globals::report.b4 = false;
                globals::report.b0 = true;
                break;
            case 1:
                globals::report.b0 = false;
                globals::report.b1 = true;
                break;
            case 2:
                globals::report.b1 = false;
                globals::report.b2 = true;
                break;
            case 3:
                globals::report.b2 = false;
                globals::report.b3 = true;
                break;
            case 4:
                globals::report.b3 = false;
                globals::report.b4 = true;
                break;
            }

            globals::dummy_output_state++;
        }
        else
        {
            globals::report.hat = HatSwitchDirections::HATSWITCH_NONE;
            globals::report.b0  = false;
            globals::report.b1  = false;
            globals::report.b2  = false;
            globals::report.b3  = false;
            globals::report.b4  = false;
        }
#else
        globals::report.x  += 1;
        globals::report.y  += 2;
        globals::report.rx += 3;
        globals::report.ry += 4;
        globals::report.z  += 5;
        globals::report.rz += 6;

        uint8_t tmp         = globals::report.b18;
        globals::report.b18 = globals::report.b17;
        globals::report.b17 = globals::report.b16;
        globals::report.b16 = globals::report.b9;
        globals::report.b9  = globals::report.b8;
        globals::report.b8  = globals::report.b7;
        globals::report.b7  = globals::report.b6;
        globals::report.b6  = globals::report.b5;
        globals::report.b5  = globals::report.b4;
        globals::report.b4  = globals::report.b3;
        globals::report.b3  = globals::report.b2;
        globals::report.b2  = globals::report.b1;
        globals::report.b1  = globals::report.b0;
        globals::report.b0  = tmp;

        globals::report.hat += 1;
        if (globals::report.hat > 7)
            globals::report.hat = 0;
#endif

        send_report(globals::report);
    }

    globals::timers::dummy_report_timer.start();
}

void dummy_start_stop_handler()
{
    if (globals::dummy_running)
    {
        printf("Stopping dummy output\n");
        globals::dummy_running = false;
    }
    else
    {
        printf("Starting dummy output for %d ms\n",
               config::dummy_stop_timeout_ms);
        globals::dummy_running = true;

        globals::timers::dummy_start_stop_timer.set_timeout(
            config::dummy_stop_timeout_ms);
        globals::timers::dummy_start_stop_timer.start();
    }
}

int main()
{
    stdio_init_all();
    printf("Hello!\r\n");

    std::memset(&globals::report, 0, sizeof(globals::report));

    cyw43_arch_init();

    init_bt();
    set_on_connected_callback([] {
        globals::dummy_output_state = 0;
        globals::timers::dummy_start_stop_timer.start();
    });

    globals::timers::dummy_report_timer.start();

    OneShotTimer t1 { [] { printf("T1\n"); }, time_to_ms(1, 500) };
    t1.start();

    printf("GO\n");
    btstack_run_loop_execute();
}
