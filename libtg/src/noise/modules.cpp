#include "noise/modules.h"

#include <glm/common.hpp>

namespace tarragon::noise
{
    NoiseFunction Abs(NoiseFunction source)
    {
        return [=](glm::dvec3 const& pos)
        {
            return glm::abs(source(pos));
        };
    }

    NoiseFunction Add(NoiseFunction source0, NoiseFunction source1)
    {
        return [=](glm::dvec3 const& pos)
        {
            return source0(pos) + source1(pos);
        };
    }

    NoiseFunction Constant(double value)
    {
        return [=](glm::dvec3 const& pos)
        {
            return value;
        };
    }

    NoiseFunction Blend(NoiseFunction source0, NoiseFunction source1, NoiseFunction control)
    {
        return [=](glm::dvec3 const& pos)
        {

        };
    }
}
