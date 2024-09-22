#pragma once

#include <array>
#include <cstddef>

template <typename T, std::size_t Cap>
class PArray
{
public:
    bool push_back(T el)
    {
        if (m_next_idx == Cap)
            return false;
        m_data[m_next_idx++] = std::move(el);
        return true;
    }

    T pop_back() {
        auto el = m_data[m_next_idx--];
        return std::move(el);
    }

    T& operator[](int idx) {
        return m_data[idx];
    }

    T* begin() {
        return m_data.begin();
    }

    T* end() {
        return &m_data[m_next_idx];
    }

    [[nodiscard]] std::size_t size() const { return m_next_idx; }

    constexpr std::size_t capacity() { return Cap; }

private:
    std::array<T, Cap> m_data;
    std::size_t      m_next_idx;
};
