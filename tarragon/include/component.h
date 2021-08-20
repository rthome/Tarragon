#pragma once

#include "clock.h"
#include "input.h"

namespace tarragon
{
    class UpdateComponent
    {
    public:
        virtual void update(Clock const& clock, Input const& input) = 0;
    };
}
