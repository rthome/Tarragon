#include "noise/modules.h"

#include <cmath>
#include <numeric>
#include <algorithm>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext/quaternion_double.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/spline.hpp>

#include "common.h"
#include "noise/common.h"
#include "noise/generator.h"

namespace
{
    struct CacheEntry
    {
        glm::dvec3 pos;
        double val;
    };

    constexpr std::array<double, tarragon::noise::RidgedMultiMaxOctaveCount> calc_ridgedmulti_spectral_weights(double lacunarity)
    {
        const double h = 1.0;
        std::array<double, tarragon::noise::RidgedMultiMaxOctaveCount> weights{};

        double frequency = 1.0;
        for (size_t i = 0; i < weights.size(); i++)
        {
            weights.at(i) = glm::pow(frequency, -h);
            frequency *= lacunarity;
        }

        return weights;
    }
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

            for (int32_t current_octave = 0; static_cast<uint32_t>(current_octave) < octave_count; current_octave++)
            {
                // Get the coherent-noise value from the input value and add it to the final result.
                int32_t octave_seed = (seed + current_octave) & INT32_MAX;
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
            thread_local CacheEntry entry{};
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
            auto ipos = glm::ivec3
            { 
                pos.x > 0 ? static_cast<int32_t>(pos.x) : static_cast<int32_t>(pos.x) - 1,
                pos.y > 0 ? static_cast<int32_t>(pos.y) : static_cast<int32_t>(pos.y) - 1,
                pos.z > 0 ? static_cast<int32_t>(pos.z) : static_cast<int32_t>(pos.z) - 1
            };

            double minDistance = DBL_MAX;
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
                        glm::dvec3 cube_pos
                        {
                            xcur + value_noise_3d(cube_ipos, seed),
                            ycur + value_noise_3d(cube_ipos, seed + 1),
                            zcur + value_noise_3d(cube_ipos, seed + 2)
                        };
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
        return [=](glm::dvec3 pos)
        {
            UNUSED_PARAM(pos);

            return value;
        };
    }

    Module Curve(Module source, ControlPoint const* control_points, size_t control_point_count)
    {
        assert(control_points != nullptr);

        std::vector<ControlPoint> sorted_control_points{control_points, control_points + control_point_count };
        std::sort(std::begin(sorted_control_points), std::end(sorted_control_points),
            [](auto& a, auto& b) { return a.Input < b.Input; });

        return [source, sorted_control_points](glm::dvec3 pos)
        {
            // Get the output value from the source module.
            auto source_value = source(pos);

            // Find the first element in the control point array that has an input value
            // larger than the output value from the source module.
            int32_t index_pos{};
            for (index_pos = 0; index_pos < sorted_control_points.size(); index_pos++)
            {
                if (source_value < sorted_control_points.at(index_pos).Input)
                    break;
            }

            // Find the four nearest control points so that we can perform cubic
            // interpolation.
            auto index0 = glm::clamp(index_pos - 2, 0, static_cast<int32_t>(sorted_control_points.size() - 1));
            auto index1 = glm::clamp(index_pos - 1, 0, static_cast<int32_t>(sorted_control_points.size() - 1));
            auto index2 = glm::clamp(index_pos, 0, static_cast<int32_t>(sorted_control_points.size() - 1));
            auto index3 = glm::clamp(index_pos + 1, 0, static_cast<int32_t>(sorted_control_points.size() - 1));

            // If some control points are missing (which occurs if the value from the
            // source module is greater than the largest input value or less than the
            // smallest input value of the control point array), get the corresponding
            // output value of the nearest control point and exit now.
            if (index1 == index2)
                return sorted_control_points.at(index1).Output;

            // Compute the alpha value used for cubic interpolation.
            auto input0 = sorted_control_points.at(index1).Input;
            auto input1 = sorted_control_points.at(index2).Input;
            auto alpha = (source_value - input0) / (input1 - input0);

            return glm::cubic(
                glm::dvec1{ sorted_control_points.at(index0).Output },
                glm::dvec1{ sorted_control_points.at(index1).Output },
                glm::dvec1{ sorted_control_points.at(index2).Output },
                glm::dvec1{ sorted_control_points.at(index3).Output },
                alpha).x;
        };
    }

    Module Cylinders(double frequency)
    {
        return [=](glm::dvec3 pos)
        {
            pos.x *= frequency;
            pos.y *= frequency;

            auto center_dist = glm::length(glm::dvec2{ pos });
            auto inner_sphere_dist = center_dist - glm::floor(center_dist);
            auto outer_sphere_dist = 1.0 - inner_sphere_dist;
            auto nearest_dist = glm::min(inner_sphere_dist, outer_sphere_dist);
            return 1.0 - (nearest_dist * 4.0); // Puts it in the -1.0 to +1.0 range.
        };
    }

    Module Displace(Module source, Module xdisplace, Module ydisplace, Module zdisplace)
    {
        return [=](glm::dvec3 pos)
        {
            auto displaced_pos = pos +
                glm::dvec3{ xdisplace(pos), ydisplace(pos), zdisplace(pos) };
            return source(displaced_pos);
        };
    }

    Module Exponent(Module source, double exponent)
    {
        return [=](glm::dvec3 pos)
        {
            auto value = source(pos);
            return glm::pow(glm::abs((value + 1) / 2), exponent) * 2 - 1;
        };
    }

    Module Multiply(Module source0, Module source1)
    {
        return [=](glm::dvec3 pos)
        {
            return source0(pos) * source1(pos);
        };
    }

    Module Invert(Module source)
    {
        return [=](glm::dvec3 pos)
        {
            return -1.0 * source(pos);
        };
    }

    Module Max(Module source0, Module source1)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::max(source0(pos), source1(pos));
        };
        
    }

    Module Min(Module source0, Module source1)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::min(source0(pos), source1(pos));
        };
    }

    Module Perlin(double frequency, double lacunarity, uint32_t octave_count, double persistence, NoiseQuality quality, int32_t seed)
    {
        return [=](glm::dvec3 pos)
        {
            double value{};
            double current_persistence = 1.0;

            pos *= frequency;

            for (int32_t octave = 0; static_cast<uint32_t>(octave) < octave_count; octave++)
            {
                int32_t octave_seed = (seed + octave) & INT32_MAX;
                auto signal = gradient_coherent_noise_3d(pos, octave_seed, quality);
                value += signal * current_persistence;

                pos *= lacunarity;
                current_persistence *= persistence;
            }

            return value;
        };
    }

    Module Power(Module source0, Module source1)
    {
        return [=](glm::dvec3 pos)
        {
            return glm::pow(source0(pos), source1(pos));
        };
    }

    Module RidgedMulti(double frequency, double lacunarity, uint32_t octave_count, NoiseQuality quality, int32_t seed)
    {
        assert(octave_count <= RidgedMultiMaxOctaveCount);

        auto spectral_weights = calc_ridgedmulti_spectral_weights(lacunarity);
        const double offset = 1.0;
        const double gain = 2.0;

        return [=](glm::dvec3 pos)
        {
            double value{};
            double weight = 1.0;

            pos *= frequency;

            for (int32_t octave = 0; static_cast<uint32_t>(octave) < octave_count; octave++)
            {
                // Get the coherent-noise value.
                int32_t octave_seed = (seed + octave) & 0x7fffffff;
                auto signal = gradient_coherent_noise_3d(pos, octave_seed, quality);

                // Make the ridges.
                signal = offset - glm::abs(signal);

                // Square the signal to increase the sharpness of the ridges.
                signal *= signal;

                // The weighting from the previous octave is applied to the signal.
                // Larger values have higher weights, producing sharp points along the
                // ridges.
                signal *= weight;

                // Weight successive contributions by the previous signal.
                weight = signal * gain;
                if (weight > 1.0)
                    weight = 1.0;
                if (weight < 0.0)
                    weight = 0.0;

                // Add the signal to the output value.
                value += (signal * spectral_weights[octave]);

                pos *= lacunarity;
            }

            return (value * 1.25) - 1.0;
        };
    }

    Module Rotate(Module source, double xdegrees, double ydegrees, double zdegrees)
    {
        constexpr glm::dquat quat_id = glm::identity<glm::quat>();

        auto qx = glm::rotate(quat_id, glm::radians(xdegrees), glm::dvec3{ 1.0, 0.0, 0.0 });
        auto qy = glm::rotate(quat_id, glm::radians(ydegrees), glm::dvec3{ 0.0, 1.0, 0.0 });
        auto qz = glm::rotate(quat_id, glm::radians(zdegrees), glm::dvec3{ 0.0, 0.0, 1.0 });

        auto rotation = qx * qy * qz;

        return [source, rotation](glm::dvec3 pos)
        {
            auto rotated_pos = rotation * pos;
            return source(rotated_pos);
        };
    }

    Module ScaleBias(Module source, double scale, double bias)
    {
        return [=](glm::dvec3 pos)
        {
            return source(pos) * scale + bias;
        };
    }

    Module ScalePoint(Module source, glm::dvec3 const& scale_factor)
    {
        return [=](glm::dvec3 pos)
        {
            return source(pos * scale_factor);
        };
    }

    Module Select(Module source0, Module source1, Module control,
        double lower_bound, double upper_bound, double edge_falloff)
    {
        return [=](glm::dvec3 pos)
        {
            auto control_value = control(pos);
            if (edge_falloff > 0.0)
            {
                if (control_value < (lower_bound - edge_falloff))
                {
                    // The output value from the control module is below the selector
                    // threshold; return the output value from the first source module.
                    return source0(pos);
                }
                else if (control_value < (lower_bound + edge_falloff))
                {
                    // The output value from the control module is near the lower end of the
                    // selector threshold and within the smooth curve. Interpolate between
                    // the output values from the first and second source modules.
                    auto lower_curve = lower_bound - edge_falloff;
                    auto upper_curve = lower_bound + edge_falloff;
                    auto alpha = scurve3((control_value - lower_curve) / (upper_curve / lower_curve));
                    return glm::mix(source0(pos), source1(pos), alpha);
                }
                else if (control_value < (upper_bound - edge_falloff))
                {
                    // The output value from the control module is within the selector
                    // threshold; return the output value from the second source module.
                    return source1(pos);
                }
                else if (control_value < (upper_bound + edge_falloff))
                {
                    // The output value from the control module is near the upper end of the
                    // selector threshold and within the smooth curve. Interpolate between
                    // the output values from the first and second source modules.
                    auto lower_curve = upper_bound - edge_falloff;
                    auto upper_curve = upper_bound + edge_falloff;
                    auto alpha = scurve3((control_value - lower_curve) / (upper_curve / lower_curve));
                    return glm::mix(source1(pos), source0(pos), alpha);
                }
                else
                {
                    // Output value from the control module is above the selector threshold;
                    // return the output value from the first source module.
                    return source0(pos);
                }
            }
            else
            {
                if (control_value < lower_bound || control_value > upper_bound)
                    return source0(pos);
                else
                    return source1(pos);
            }
        };
    }

    Module Spheres(double frequency)
    {
        return [=](glm::dvec3 pos)
        {
            pos *= frequency;

            auto center_dist = glm::length(pos);
            auto inner_sphere_dist = center_dist - glm::floor(center_dist);
            auto outer_sphere_dist = 1.0 - inner_sphere_dist;
            auto nearest_dist = glm::min(inner_sphere_dist, outer_sphere_dist);
            return 1.0 - (nearest_dist * 4.0); // Puts it in the -1.0 to +1.0 range.
        };
    }

    Module Terrace(Module source, double const* control_points, size_t control_point_count, bool invert_terraces)
    {
        assert(control_points != nullptr);

        std::vector<double> control_point_vec{ control_points, control_points + control_point_count };

        return [source, control_point_vec, invert_terraces](glm::dvec3 pos)
        {
            auto source_value = source(pos);

            // Find the first element in the control point array that has a value
            // larger than the output value from the source module.
            size_t index{};
            for (index = 0; index < control_point_vec.size(); index++)
            {
                if (source_value < control_point_vec.at(index))
                    break;
            }

            // Find the two nearest control points so that we can map their values
            // onto a quadratic curve.
            auto index0 = glm::clamp(index - 1, 0ull, control_point_vec.size() - 1);
            auto index1 = glm::clamp(index, 0ull, control_point_vec.size() - 1);

            // If some control points are missing (which occurs if the output value from
            // the source module is greater than the largest value or less than the
            // smallest value of the control point array), get the value of the nearest
            // control point and exit now.
            if (index0 == index1)
                return control_point_vec.at(index1);

            // Compute the alpha value used for linear interpolation.
            auto value0 = control_point_vec.at(index0);
            auto value1 = control_point_vec.at(index1);
            auto alpha = (source_value - value0) / (value1 - value0);
            if (invert_terraces)
            {
                alpha = 1.0 - alpha;
                std::swap(value0, value1);
            }

            // Squaring the alpha produces the terrace effect.
            alpha *= alpha;

            // Now perform the linear interpolation given the alpha value.
            return glm::mix(value0, value1, alpha);
        };
    }

    Module TranslatePoint(Module source, glm::dvec3 const& translation)
    {
        return [=](glm::dvec3 pos)
        {
            return source(pos + translation);
        };
    }

    Module Turbulence(Module source, double frequency, double power, double roughness, int32_t seed)
    {
        auto xdistort = Perlin(frequency, PerlinDefaultLacunarity, roughness, PerlinDefaultPersistence, DefaultQuality, seed);
        auto ydistort = Perlin(frequency, PerlinDefaultLacunarity, roughness, PerlinDefaultPersistence, DefaultQuality, seed + 1);
        auto zdistort = Perlin(frequency, PerlinDefaultLacunarity, roughness, PerlinDefaultPersistence, DefaultQuality, seed + 2);
        
        static constexpr glm::dvec3 offset0{ 12414.0 / 65536.0, 65124.0 / 65536.0, 31337.0 / 65536.0 };
        static constexpr glm::dvec3 offset1{ 26519.0 / 65536.0, 18128.0 / 65536.0, 60493.0 / 65536.0 };
        static constexpr glm::dvec3 offset2{ 53820.0 / 65536.0, 11213.0 / 65536.0, 44845.0 / 65536.0 };

        return [=](glm::dvec3 pos)
        {
            // Get the values from the three Perlin noise modules and
            // add each value to each coordinate of the input value. There are also
            // some offsets added to the coordinates of the input values. This prevents
            // the distortion modules from returning zero if the (x, y, z) coordinates,
            // when multiplied by the frequency, are near an integer boundary. This is
            // due to a property of gradient coherent noise, which returns zero at
            // integer boundaries.
            glm::dvec3 pos0{ pos + offset0 }, pos1{ pos + offset1 }, pos2{ pos + offset2 };
            glm::dvec3 distorted_pos = pos + power * glm::dvec3{ xdistort(pos0), ydistort(pos1), zdistort(pos2) };
            return source(distorted_pos);
        };
    }

    Module White(int32_t scale, int32_t seed)
    {
        return [=](glm::dvec3 pos)
        {
            return value_noise_3d(glm::ivec3{ pos * static_cast<double>(scale) }, seed);
        };
    }
}
