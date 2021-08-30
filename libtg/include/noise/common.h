#pragma once

namespace tarragon::noise
{
    template <typename T>
    constexpr T scurve3(T a)
    {
        return a * a * (T{ 3 } - T{ 2 } *a);
    }

    template <typename T>
    constexpr T scurve5(T a)
    {
        T a3 = a * a * a;
        T a4 = a3 * a;
        T a5 = a4 * a;
        return (T{ 6 } *a5) - (T{ 15 } *a4) + (T{ 10 } *a3);
    }
}
