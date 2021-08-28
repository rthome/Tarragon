#pragma once

#include "clock.h"

namespace tarragon
{
    class System
    {

    };

    class Component
    {
    public:
        virtual void initialize() = 0;
    };

    class UpdateComponent : public Component
    {
    public:
        virtual void update(Clock const& clock) = 0;
    };

    class DrawComponent : public Component
    {
    public:
        virtual void draw() = 0;
    };
}
