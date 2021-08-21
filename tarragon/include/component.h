#pragma once

#include "clock.h"

namespace tarragon
{
    class System
    {

    };

    class UpdateComponent
    {
    public:
        virtual void update(Clock const& clock) = 0;
    };
}
