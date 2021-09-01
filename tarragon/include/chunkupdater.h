#pragma once

#include <array>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "component.h"
#include "chunk.h"
#include "chunktransfer.h"

namespace tarragon
{
    class ChunkUpdater : public UpdateComponent
    {
    public:
        ChunkUpdater() = default;
        virtual ~ChunkUpdater() = default;
        ChunkUpdater(ChunkUpdater const&) = delete;
        ChunkUpdater& operator= (ChunkUpdater const&) = delete;

        virtual void initialize() override
        {
            
        }

        virtual void update(Clock const& clock) override;
    };
}
