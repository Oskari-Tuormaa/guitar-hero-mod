#include "timer.hpp"

OneShotTimer::OneShotTimer()
    : OneShotTimer { nullptr, 0 }
{ }

OneShotTimer::OneShotTimer(TimerCallback callback, uint32_t timeout_ms)
    : m_ts {}
    , m_callback { std::move(callback) }
    , m_timeout_ms { timeout_ms }
{
    btstack_run_loop_set_timer_context(&m_ts, this);
}

void OneShotTimer::start()
{
    if (!m_callback)
    {
        return;
    }
    btstack_run_loop_set_timer_handler(&m_ts, timer_dispatch);
    btstack_run_loop_set_timer(&m_ts, m_timeout_ms);
    btstack_run_loop_add_timer(&m_ts);
}

void OneShotTimer::stop()
{
    btstack_run_loop_remove_timer(&m_ts);
}

void OneShotTimer::restart()
{
    stop();
    start();
}

void OneShotTimer::set_callback(TimerCallback callback)
{
    m_callback = std::move(callback);
}

void OneShotTimer::set_timeout(uint32_t timeout_ms)
{
    m_timeout_ms = timeout_ms;
}

void OneShotTimer::timer_dispatch(btstack_timer_source_t* ts)
{
    void* context        = btstack_run_loop_get_timer_context(ts);
    auto* timer_instance = static_cast<OneShotTimer*>(context);
    timer_instance->m_callback();
}

RepeatingTimer::RepeatingTimer()
    : RepeatingTimer { nullptr, 0 }
{ }

RepeatingTimer::RepeatingTimer(TimerCallback callback, uint32_t timeout_ms)
    : m_timer { nullptr, timeout_ms }
{
    set_callback(std::move(callback));
}

void RepeatingTimer::start()
{
    m_timer.start();
}

void RepeatingTimer::stop()
{
    m_timer.stop();
}

void RepeatingTimer::restart()
{
    m_timer.restart();
}

void RepeatingTimer::set_callback(TimerCallback callback)
{
    m_callback = std::move(callback);
    m_timer.set_callback([this] {
        start();
        m_callback();
    });
}

void RepeatingTimer::set_timeout(uint32_t timeout_ms)
{
    m_timer.set_timeout(timeout_ms);
}
