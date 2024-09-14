#pragma once

#include <btstack.h>

#include "inplace_function.hpp"

using TimerCallback = stdext::inplace_function<void()>;

class OneShotTimer
{
public:
    OneShotTimer();
    OneShotTimer(TimerCallback callback, uint32_t timeout_ms);

    void start();
    void stop();
    void restart();
    void set_callback(TimerCallback callback);
    void set_timeout(uint32_t timeout_ms);

private:
    static void timer_dispatch(btstack_timer_source_t* ts);

    btstack_timer_source_t m_ts;
    TimerCallback          m_callback;
    uint32_t               m_timeout_ms;
};

class RepeatingTimer
{
public:
    RepeatingTimer();
    RepeatingTimer(TimerCallback callback, uint32_t timeout_ms);

    void start();
    void stop();
    void restart();
    void set_callback(TimerCallback callback);
    void set_timeout(uint32_t timeout_ms);

private:
    TimerCallback m_callback;
    OneShotTimer  m_timer;
};

template<typename T>
constexpr T sec_to_ms(T sec)
{
    return sec * 1'000;
}

template<typename T>
constexpr T min_to_ms(T min)
{
    return sec_to_ms(min * 60);
}

template<typename T>
constexpr T time_to_ms(T ms)
{
    return ms;
}

template<typename T>
constexpr T time_to_ms(T sec, T ms)
{
    return sec * 1'000 + time_to_ms(ms);
}

template<typename T>
constexpr T time_to_ms(T min, T sec, T ms)
{
    return min * 60 + time_to_ms(sec, ms);
}
