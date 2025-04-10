#include "shutdown.hpp"

#include "utils.hpp"

#include <btstack.h>

namespace shutdown
{

namespace
{

bool should_restart_timer = false;

static btstack_timer_source_t shutdown_timer;
constexpr auto                shutdown_timeout_ms = T_MIN_TO_MS(2);
void                          shutdown_timer_callback(btstack_timer_source_t* ts)
{
    btstack_run_loop_trigger_exit();
}

static btstack_timer_source_t restart_shutdown_timer;
constexpr auto                restart_shutdown_timeout_ms = T_MS(500);
void                          restart_shutdown_timer_callback(btstack_timer_source_t* ts)
{
    if (should_restart_timer)
    {
        btstack_run_loop_remove_timer(&shutdown_timer);
        btstack_run_loop_set_timer(&shutdown_timer, shutdown_timeout_ms);
        btstack_run_loop_add_timer(&shutdown_timer);
        should_restart_timer = false;
    }

    btstack_run_loop_set_timer(&restart_shutdown_timer, restart_shutdown_timeout_ms);
    btstack_run_loop_add_timer(&restart_shutdown_timer);
}

} // namespace

void restart()
{
    should_restart_timer = true;
}

void init()
{
    btstack_run_loop_set_timer_handler(&shutdown_timer, shutdown_timer_callback);
    btstack_run_loop_set_timer(&shutdown_timer, shutdown_timeout_ms);
    btstack_run_loop_add_timer(&shutdown_timer);

    btstack_run_loop_set_timer_handler(&restart_shutdown_timer, restart_shutdown_timer_callback);
    btstack_run_loop_set_timer(&restart_shutdown_timer, restart_shutdown_timeout_ms);
    btstack_run_loop_add_timer(&restart_shutdown_timer);
}

} // namespace shutdown
