#include "noise/generator.h"

#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/component_wise.hpp>

namespace tarragon::noise
{
    namespace
    {
        constexpr glm::ivec3 NoiseGen{ 1619, 31337, 6971 };
        constexpr int32_t SeedNoiseGen = 1013;
        constexpr int32_t ShiftNoiseGen = 8;

        constexpr glm::dvec3 VectorTable[256] {};

        template <typename T>
        inline constexpr T scurve3(T a)
        {
            return a * a * (T{3} - T{2} * a);
        }

        template <typename T>
        inline constexpr T scurve5(T a)
        {
            T a3 = a * a * a;
            T a4 = a3 * a;
            T a5 = a4 * a;
            return (T{6} * a5) - (T{15} * a4) + (T{10} * a3);
        }
    }

    double gradient_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed, NoiseQuality quality)
    {
        glm::ivec3 pos0 = glm::ivec3{ pos } - glm::step(0, glm::ivec3{ pos });
        glm::ivec3 pos1 = pos0 + 1;

        glm::dvec3 pos_diff = pos - glm::dvec3{ pos0 };
        glm::dvec3 spos{};
        switch (quality)
        {
            case NoiseQuality::Fast:
                spos = pos_diff;
                break;
            case NoiseQuality::Standard:
                spos = scurve3(pos_diff);
                break;
            case NoiseQuality::Best:
                spos = scurve5(pos_diff);
                break;
        }

        double ix0, ix1, iy0, iy1;
        ix0 = glm::mix(
            gradient_noise_3d(pos, glm::dvec3{ pos0.x, pos0.y, pos0.z }, seed),
            gradient_noise_3d(pos, glm::dvec3{ pos1.x, pos0.y, pos0.z }, seed),
            spos.x);
        ix1 = glm::mix(
            gradient_noise_3d(pos, glm::dvec3{ pos0.x, pos1.y, pos0.z }, seed),
            gradient_noise_3d(pos, glm::dvec3{ pos1.x, pos1.y, pos0.z }, seed),
            spos.x);
        iy0 = glm::mix(ix0, ix1, spos.y);
        ix0 = glm::mix(
            gradient_noise_3d(pos, glm::dvec3{ pos0.x, pos0.y, pos1.z }, seed),
            gradient_noise_3d(pos, glm::dvec3{ pos1.x, pos0.y, pos1.z }, seed),
            spos.x);
        ix1 = glm::mix(
            gradient_noise_3d(pos, glm::dvec3{ pos0.x, pos1.y, pos1.z }, seed),
            gradient_noise_3d(pos, glm::dvec3{ pos1.x, pos1.y, pos1.z }, seed),
            spos.x);
        iy1 = glm::mix(ix0, ix1, spos.y);
        return glm::mix(iy0, iy1, spos.z);
    }

    double gradient_noise_3d(glm::dvec3 const& fpos, glm::ivec3 const& ipos, int32_t seed)
    {
        int vector_index = (glm::compAdd(NoiseGen * ipos) + (SeedNoiseGen * seed)) & 0xffffffff;
        vector_index ^= (vector_index >> ShiftNoiseGen);
        vector_index &= 0xff;

        glm::dvec3 vgrad = VectorTable[vector_index];
        glm::dvec3 vpoint = fpos - glm::dvec3{ ipos };

        return glm::dot(vgrad, vpoint) * 2.12;
    }
    
    double value_coherent_noise_3d(glm::dvec3 const& pos, int32_t seed, NoiseQuality quality)
    {
        glm::ivec3 pos0 = glm::ivec3{ pos } - glm::step(0, glm::ivec3{ pos });
        glm::ivec3 pos1 = pos0 + 1;

        glm::dvec3 pos_diff = pos - glm::dvec3{ pos0 };
        glm::dvec3 spos{};
        switch (quality)
        {
            case NoiseQuality::Fast:
                spos = pos_diff;
                break;
            case NoiseQuality::Standard:
                spos = scurve3(pos_diff);
                break;
            case NoiseQuality::Best:
                spos = scurve5(pos_diff);
                break;
        }

        double ix0, ix1, iy0, iy1;
        ix0 = glm::mix(
            value_noise_3d(glm::ivec3{ pos0.x, pos0.y, pos0.z }, seed),
            value_noise_3d(glm::ivec3{ pos1.x, pos0.y, pos0.z }, seed),
            spos.x);
        ix1 = glm::mix(
            value_noise_3d(glm::ivec3{ pos0.x, pos1.y, pos0.z }, seed),
            value_noise_3d(glm::ivec3{ pos1.x, pos1.y, pos0.z }, seed),
            spos.x);
        iy0 = glm::mix(ix0, ix1, spos.y);
        ix0 = glm::mix(
            value_noise_3d(glm::ivec3{ pos0.x, pos0.y, pos1.z }, seed),
            value_noise_3d(glm::ivec3{ pos1.x, pos0.y, pos1.z }, seed),
            spos.x);
        ix1 = glm::mix(
            value_noise_3d(glm::ivec3{ pos0.x, pos1.y, pos1.z }, seed),
            value_noise_3d(glm::ivec3{ pos1.x, pos1.y, pos1.z }, seed),
            spos.x);
        iy1 = glm::mix(ix0, ix1, spos.y);
        return glm::mix(iy0, iy1, spos.z);
    }

    double value_noise_3d(glm::ivec3 const& pos, int32_t seed)
    {
        return 1.0 - (static_cast<double>(int_value_noise_3d(pos, seed)) / 1073741824.0);
    }

    int32_t int_value_noise_3d(glm::ivec3 const& pos, int32_t seed)
    {
        auto n = (glm::compAdd(NoiseGen * pos) + (SeedNoiseGen * seed)) & 0x7fffffff;
        n = (n >> 13) ^ n;
        return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    }
}
