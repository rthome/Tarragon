#include "gmock/gmock.h"

#include <noise/modules.h>

using namespace testing;
using namespace tarragon::noise;

namespace tarragon::tests
{
    TEST(NoiseModuleTests, Abs)
    {
        auto constant_0 = Constant(0.0);
        auto constant_1 = Constant(1.0);
        auto constant_m1 = Constant(-1.0);

        ASSERT_THAT(Abs(constant_0)({0.0, 0.0, 0.0}), Eq(0.0));
        ASSERT_THAT(Abs(constant_1)({0.0, 0.0, 0.0}), Eq(1.0));
        ASSERT_THAT(Abs(constant_m1)({0.0, 0.0, 0.0}), Eq(1.0));
    }

    TEST(NoiseModuleTests, Constant)
    {
        auto default_constant = Constant();
        auto constant1 = Constant(1.0);

        ASSERT_THAT(default_constant({0.0, 0.0, 0.0}), Eq(ConstantDefaultValue));
        ASSERT_THAT(default_constant({1.0, 10.0, 100.0}), Eq(ConstantDefaultValue));
        ASSERT_THAT(constant1({0.0, 0.0, 0.0}), Eq(1.0));
        ASSERT_THAT(constant1({1.0, 10.0, 100.0}), Eq(1.0));
    }

    TEST(NoiseModuleTests, Add)
    {
        auto source0 = Constant(1.0);
        auto source1 = Constant(4.0);
        auto add = Add(source0, source1);

        ASSERT_THAT(add({0.0, 0.0, 0.0}), Eq(5.0));
    }

    TEST(NoiseModuleTests, Blend)
    {
        auto blend1 = Blend(Constant(1), Constant(2), Constant(0));
        ASSERT_THAT(blend1({}), Eq(1));

        auto blend2 = Blend(Constant(1), Constant(2), Constant(1));
        ASSERT_THAT(blend2({}), Eq(2));

        auto blend3 = Blend(Constant(1), Constant(2), Constant(0.5));
        ASSERT_THAT(blend3({}), Eq(1.5));
    }
}
