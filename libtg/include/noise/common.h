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

    enum class NoiseQuality
    {
        // Generates coherent noise quickly.When a coherent-noise function with
        // this quality setting is used to generate a bump-map image, there are
        // noticeable "creasing" artifacts in the resulting image. This is
        // because the derivative of that function is discontinuous at integer
        // boundaries.
        Fast,

        // Generates standard-quality coherent noise. When a coherent-noise
        // function with this quality setting is used to generate a bump-map
        // image, there are some minor "creasing" artifacts in the resulting
        // image. This is because the second derivative of that function is
        // discontinuous at integer boundaries.
        Standard,

        // Generates the best-quality coherent noise. When a coherent-noise
        // function with this quality setting is used to generate a bump-map
        // image, there are no "creasing" artifacts in the resulting image. This
        // is because the first and second derivatives of that function are
        // continuous at integer boundaries.
        Best,
    };
}
