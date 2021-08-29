#pragma once

#include <cstdint>
#include <functional>

#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

#include "generator.h"

namespace tarragon::noise
{
    using Module = std::function<double(glm::dvec3)>;

    enum class CellType
    {
        Voronoi,
        Euclidean = Voronoi,
        Quadratic,
        Manhattan,
        Chebychev,
        Minkowsky
    };

    struct ControlPoint
    {
        double Input;
        double Output;
    };

    constexpr NoiseQuality DefaultQuality = NoiseQuality::Standard;
    constexpr int32_t DefaultSeed = 0;

    constexpr double BillowDefaultFrequency = 1.0;
    constexpr double BillowDefaultLacunarity = 2.0;
    constexpr uint32_t BillowDefaultOctaveCount = 6;
    constexpr double BillowDefaultPersistence = 0.5;

    constexpr CellType CellDefaultType = CellType::Voronoi;
    constexpr double CellDefaultDisplacement = 1.0;
    constexpr double CellDefaultFrequency = 1.0;
    constexpr bool CellDefaultEnableDistance = false;
    constexpr double CellDefaultMinkowskyCoefficient = 4.0;

    constexpr double ClampDefaultLowerBound = -1.0;
    constexpr double ClampDefaultUpperBound = 1.0;

    constexpr double ConstantDefaultValue = 0.0;

    constexpr double CylindersDefaultFrequency = 1.0;

    constexpr double ExponentDefaultExponent = 1.0;

    constexpr double PerlinDefaultFrequency = 1.0;
    constexpr double PerlinDefaultLacunarity = 2.0;
    constexpr uint32_t PerlinDefaultOctaveCount = 6;
    constexpr double PerlinDefaultPersistence = 0.5;

    constexpr double RidgedMultiDefaultFrequency = 1.0;
    constexpr double RidgedMultiDefaultLacunarity = 2.0;
    constexpr uint32_t RidgedMultiDefaultOctaveCount = 6;
    constexpr uint32_t RidgedMultiMaxOctaveCount = 30;

    constexpr double RotateDefaultAngle = 0.0;

    constexpr double ScaleBiasDefaultScale = 1.0;
    constexpr double ScaleBiasDefaultBias = 0.0;

    constexpr glm::dvec3 ScalePointDefaultScaleFactor = glm::one<glm::dvec3>();

    constexpr double SelectDefaultLowerBound = -1.0;
    constexpr double SelectDefaultUpperBound = 1.0;
    constexpr double SelectDefaultEdgeFalloff = 0.0;

    constexpr double SimplexDefaultFrequency = 1.0;
    constexpr double SimplexDefaultLacunarity = 2.0;
    constexpr uint32_t SimplexDefaultOctaveCount = 6;
    constexpr double SimplexDefaultPersistence = 0.5;

    constexpr double SpheresDefaultFrequency = 1.0;

    constexpr bool TerraceDefaultInvertTerraces = false;

    constexpr glm::dvec3 TranslateDefaultTranslation = glm::zero<glm::dvec3>();

    constexpr double TurbulenceDefaultFrequency = PerlinDefaultFrequency;
    constexpr double TurbulenceDefaultPower = 1.0;
    constexpr int32_t TurbulenceDefaultRoughness = 3;

    constexpr uint32_t WhiteDefaultScale = 256;

    // Outputs the absolute value of the source value
    Module Abs(Module source);

    // Outputs the sum of the source values
    Module Add(Module source0, Module source1);

    // Generates "billowy" noise suitable for clouds and rocks
    Module Billow(
        double frequency = BillowDefaultFrequency,
        double lacunarity = BillowDefaultLacunarity,
        uint32_t octave_count = BillowDefaultOctaveCount,
        double persistence = BillowDefaultPersistence,
        NoiseQuality quality = DefaultQuality,
        int32_t seed = DefaultSeed);

    // Outputs a weighted blend of the output values from two sources,
    // given the output value supplied by a control source
    Module Blend(Module source0, Module source1, Module control);
    
    // Caches the last output value generated by its source (per thread)
    Module Cache(Module source);
    
    // Outputs cell noise
    Module Cell(
        CellType type = CellDefaultType,
        double displacement = CellDefaultDisplacement,
        double frequency = CellDefaultFrequency,
        bool enable_distance = CellDefaultEnableDistance,
        double minkowsky_coefficient = CellDefaultMinkowskyCoefficient,
        int32_t seed = DefaultSeed);

    // Outputs a checkerboard pattern
    Module Checkerboard();

    // Outputs source values clamped into a range
    Module Clamp(
        Module source,
        double lower_bound = ClampDefaultLowerBound,
        double upper_bound = ClampDefaultUpperBound);

    // Outputs a constant value
    Module Constant(double value = ConstantDefaultValue);

    // Maps the output value from a source module onto an
    // arbitrary function curve created by the given control points
    Module Curve(Module source, ControlPoint const *control_points, size_t control_point_count);

    // Outputs concentric cylinders
    Module Cylinders(double frequency = CylindersDefaultFrequency);

    // Uses three source modules to displace each coordinate 
    // of the input value before returning the output value from
    // a source module.
    Module Displace(
        Module source,
        Module xdisplace,
        Module ydisplace,
        Module zdisplace);

    // Maps the output value from a source module onto an exponential curve
    Module Exponent(Module source, double exponent = ExponentDefaultExponent);

    // Inverts the output value from a source module
    Module Invert(Module source);

    // Outputs the larger of the two output values from two source modules
    Module Max(Module source0, Module source1);

    // Outputs the smaller of the two output values from two source modules
    Module Min(Module source0, Module source1);

    // Outputs the product of the two output values from two source modules
    Module Multiply(Module source0, Module source1);

    // Outputs 3-dimensional Perlin noise
    Module Perlin(
        double frequency = PerlinDefaultFrequency,
        double lacunarity = PerlinDefaultLacunarity,
        uint32_t octave_count = PerlinDefaultOctaveCount,
        double persistence = PerlinDefaultPersistence,
        NoiseQuality quality = DefaultQuality,
        int32_t seed = DefaultSeed);
    
    // Raises the output value from a first source module
    // to the power of the output value from a second source module
    Module Power(Module source0, Module source1);

    // Outputs 3-dimensional ridged-multifractal noise
    Module RidgedMulti(
        double frequency = RidgedMultiDefaultFrequency,
        double lacunarity = RidgedMultiDefaultLacunarity,
        uint32_t octave_count = RidgedMultiDefaultOctaveCount,
        NoiseQuality quality = DefaultQuality,
        int32_t seed = DefaultSeed);

    // Rotates the input value around the origin before
    // returning the output value from a source module
    Module Rotate(
        Module source,
        double xdegrees = RotateDefaultAngle,
        double ydegrees = RotateDefaultAngle,
        double zdegrees = RotateDefaultAngle);
    
    // Applies a scaling factor and a bias to the output value from a source module
    Module ScaleBias(
        Module source,
        double scale = ScaleBiasDefaultScale,
        double bias = ScaleBiasDefaultBias);

    // Scales the coordinates of the input value before
    // returning the output value from a source module
    Module ScalePoint(
        Module source,
        glm::dvec3 const& scale_factor = ScalePointDefaultScaleFactor);

    // Outputs the value selected from one of two source
    // modules chosen by the output value from a control module
    Module Select(
        Module source0,
        Module source1,
        Module control,
        double lower_bound = SelectDefaultLowerBound,
        double upper_bound = SelectDefaultUpperBound,
        double edge_falloff = SelectDefaultEdgeFalloff);

    // Outputs 3-dimensional Simplex noise
    Module Simplex(
        double frequency = SimplexDefaultFrequency,
        double lacunarity = SimplexDefaultLacunarity,
        uint32_t octave_count = SimplexDefaultOctaveCount,
        double persistence = SimplexDefaultPersistence,
        NoiseQuality quality = DefaultQuality,
        int32_t seed = DefaultSeed);
    
    // Outputs concentric spheres
    Module Spheres(double frequency = SpheresDefaultFrequency);

    // Maps the output value from a source module onto a terrace-forming curve
    Module Terrace(
        Module source,
        double const *control_points,
        size_t control_point_count,
        bool invert_terraces = TerraceDefaultInvertTerraces);

    // Moves the coordinates of the input value before
    // returning the output value from a source module
    Module TranslatePoint(
        Module source,
        glm::dvec3 const& translation = TranslateDefaultTranslation);

    // Randomly displaces the input value before
    // returning the output value from a source module
    Module Turbulence(
        Module source,
        double frequency = TurbulenceDefaultFrequency,
        double power = TurbulenceDefaultPower,
        double roughness = TurbulenceDefaultRoughness,
        int32_t seed = DefaultSeed);

    // Outputs 3-dimensional White noise
    Module White(
        uint32_t scale = WhiteDefaultScale,
        int32_t seed = DefaultSeed);
}
