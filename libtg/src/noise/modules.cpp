#include "noise/modules.h"

#include <cmath>
#include <numeric>

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtx/component_wise.hpp>

#include "noise/generator.h"

namespace
{
    struct CacheEntry
    {
        glm::dvec3 pos;
        double val;
    };
}

namespace tarragon::noise
{
    Module Abs(Module source)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::abs(source(pos));
        };
    }

    Module Add(Module source0, Module source1)
    {
        return [=](glm::dvec3 pos)
        {
            return source0(pos) + source1(pos);
        };
    }

    Module Billow(double frequency, double lacunarity, uint32_t octave_count, double persistence, NoiseQuality quality, int32_t seed)
    {
        return [=](glm::dvec3 pos)
        {
            double value = 0.0;
            double current_persistence = 1.0;

            pos *= frequency;

            for (size_t current_octave = 0; current_octave < octave_count; current_octave++)
            {
                // Get the coherent-noise value from the input value and add it to the final result.
                auto octave_seed = (int32_t)((seed + current_octave) & std::numeric_limits<size_t>::max());
                auto signal = gradient_coherent_noise_3d(pos, octave_seed, quality);
                signal = 2.0 * glm::abs(signal) - 1.0;
                value += signal * current_persistence;

                // Prepare the next octave.
                pos *= lacunarity;
                current_persistence *= persistence;
            }
            value += 0.5;

            return value;
        };
    }

    Module Blend(Module source0, Module source1, Module control)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::mix(source0(pos), source1(pos), control(pos));
        };
    }

    Module Cache(Module source)
    {
        return [=](glm::dvec3 pos)
        {
            thread_local CacheEntry entry;
            if (pos == entry.pos)
                return entry.val;
            else
            {
                auto val = source(pos);
                entry = {pos, val};
                return val;
            }
        };
    }

    Module Cell(CellType type, double displacement, double frequency, bool enable_distance, double minkowsky_coefficient, int32_t seed)
    {
        return [=](glm::dvec3 pos)
        {
            pos *= frequency;
            auto ipos = glm::ivec3{ 
                pos.x > 0 ? (int)pos.x : (int)pos.x - 1,
                pos.y > 0 ? (int)pos.y : (int)pos.y - 1,
                pos.z > 0 ? (int)pos.z : (int)pos.z - 1 };

            double minDistance = std::numeric_limits<double>::max();
            glm::dvec3 candidate{};

            // Inside each unit cube, there is a seed point at a random position.  Go
            // through each of the nearby cubes until we find a cube with a seed point
            // that is closest to the specified position.
            for (auto zcur = ipos.z - 2; zcur <= ipos.z + 2; zcur++)
            {
                for (auto ycur = ipos.y - 2; ycur <= ipos.y + 2; ycur++)
                {
                    for (auto xcur = ipos.x - 2; xcur <= ipos.x + 2; xcur++)
                    {
                        // Calculate the position and distance to the seed point inside of
                        // this unit cube.
                        glm::dvec3 cube_ipos{xcur, ycur, zcur};
                        glm::dvec3 cube_pos{
                            xcur + value_noise_3d(cube_ipos, seed),
                            ycur + value_noise_3d(cube_ipos, seed + 1),
                            zcur + value_noise_3d(cube_ipos, seed + 2) };
                        auto dist_vec = cube_pos - pos;

                        double dist{};
                        switch (type)
                        {
                            case CellType::Voronoi:
                                dist = glm::dot(dist_vec, dist_vec);
                                break;

                            case CellType::Quadratic:
                                dist = glm::dot(dist_vec, dist_vec) + glm::dot(glm::xxy(dist_vec), glm::yzz(dist_vec));
                                break;

                            case CellType::Manhattan:
                                dist = glm::compAdd(glm::abs(dist_vec));
                                break;

                            case CellType::Chebychev:
                                dist = glm::compMax(glm::abs(dist_vec));
                                break;

                            case CellType::Minkowsky:
                            {
                                auto dd = glm::pow(glm::abs(dist_vec), glm::dvec3{ minkowsky_coefficient });
                                dist = std::pow(glm::compAdd(dd), 1.0 / minkowsky_coefficient);
                                break;
                            }
                        }

                        if (dist < minDistance)
                        {
                            // This seed point is closer to any others found so far, so record this seed point.
                            minDistance = dist;
                            candidate = cube_pos;
                        }
                    }
                }
            }

            double value{};
            if (enable_distance)
            {
                // Determine the distance to the nearest seed point.
                value = glm::distance(candidate, pos) * glm::root_three<double>() - 1.0;
            }

            // Return the calculated distance with the displacement value applied.
            return value + (displacement * value_noise_3d(glm::ivec3{ glm::floor(candidate) }));
        };
    }

    Module Checkerboard()
    {
        return [=](glm::dvec3 pos)
        {
            glm::ivec3 ipos{ glm::floor(pos) };
            auto ipos1 = ipos & 1;
            return (ipos1.x ^ ipos1.y ^ ipos1.z) != 0 ? -1.0 : 1.0;
        };
    }

    Module Clamp(Module source, double lower_bound, double upper_bound)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::clamp(source(pos), lower_bound, upper_bound);
        };
    }

    Module Constant(double value)
    {
        return [=](glm::dvec3)
        {
            return value;
        };
    }
}
