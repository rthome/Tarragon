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

    double gradient_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed = 0, NoiseQuality quality = NoiseQuality::Standard);
    double gradient_noise_3d(glm::dvec3 const& fpos, glm::ivec3 const& ipos, int32_t seed = 0);
    double value_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed = 0, NoiseQuality quality = NoiseQuality::Standard);
    double value_noise_3d(glm::ivec3 const& pos, int32_t seed = 0);
    int32_t int_value_noise_3d(glm::ivec3 const& pos, int32_t seed = 0);
}
