#pragma once

#include <inplace_function.hpp>

#include "parray.hpp"

namespace detail
{
    constexpr std::size_t max_sub_count = 20;
} // namespace detail

template <typename T>
class PubSub
{
public:
    using Callback = stdext::inplace_function<void(const T&)>;

    static void publish(const T& val)
    {
        for (auto& cb : m_subscribers)
        {
            cb(val);
        }
    }

    static void subscribe(const Callback cb) {
        m_subscribers.push_back(cb);
    }

private:
    static PArray<Callback, detail::max_sub_count> m_subscribers;
};

template <typename T>
PArray<typename PubSub<T>::Callback, detail::max_sub_count>
    PubSub<T>::m_subscribers = {};
