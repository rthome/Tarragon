#pragma once

#include <cstdint>

#define UNUSED_PARAM(p) ((void)(p))

namespace tarragon
{
    template <typename T = int32_t>
    class IncId final
    {
    private:
        T m_ctr{};

    public:
        T get() { return m_ctr++; }
        T operator()() { return get(); }

        void reset() { m_ctr = {}; }
    };
}
