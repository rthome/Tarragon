#pragma once

#include <cstdint>

#include <glm/vec3.hpp>

namespace tarragon::noise
{
    enum class NoiseQuality
    {
        Fast,
        Standard,
        Best,
    };

    // Generates a gradient-coherent-noise value from the coordinates of a
    // three-dimensional input value.
    //
    // The return value ranges from -1.0 to +1.0.
    //
    // For an explanation of the difference between gradient noise and
    // value noise, see the comments for the gradient_noise_3d function.
    double gradient_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed = 0, NoiseQuality quality = NoiseQuality::Standard);
    
    // Generates a gradient-noise value from the coordinates of a
    // three-dimensional input value and the integer coordinates of a
    // nearby three-dimensional value.
    //
    // The difference between fpos and ipos must be less than or equal to one.
    // 
    // A gradient-noise function generates better-quality noise than a
    // value-noise function.  Most noise modules use gradient noise for
    // this reason, although it takes much longer to calculate.
    //
    // The return value ranges from -1.0 to +1.0.
    //
    // This function generates a gradient-noise value by performing the
    // following steps:
    // - It first calculates a random normalized vector based on the
    //   nearby integer value passed to this function.
    // - It then calculates a new value by adding this vector to the
    //   nearby integer value passed to this function.
    // - It then calculates the dot product of the above-generated value
    //   and the floating-point input value passed to this function.
    //
    // A noise function differs from a random-number generator because it
    // always returns the same output value if the same input value is passed
    // to it.
    double gradient_noise_3d(glm::dvec3 const& fpos, glm::ivec3 const& ipos, int32_t seed = 0);
    
    // Generates a value-coherent-noise value from the coordinates of a
    // three-dimensional input value.
    //
    // The return value ranges from -1.0 to +1.0.
    //
    // For an explanation of the difference between gradient noise and
    // value noise, see the comments for the gradient_noise_3d function.
    double value_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed = 0, NoiseQuality quality = NoiseQuality::Standard);
    
    // Generates a value - noise value from the coordinates of a
    // three-dimensional input value.
    //
    // The return value ranges from -1.0 to +1.0.
    //
    // A noise function differs from a random-number generator because it
    // always returns the same output value if the same input value is passed
    // to it.
    double value_noise_3d(glm::ivec3 const& pos, int32_t seed = 0);

    // Generates an integer-noise value from the coordinates of a
    // three-dimensional input value.
    //
    // The return value ranges from 0 to 2147483647.
    // 
    // A noise function differs from a random-number generator because it
    // always returns the same output value if the same input value is passed
    // to it.
    int32_t int_value_noise_3d(glm::ivec3 const& pos, int32_t seed = 0);
}
