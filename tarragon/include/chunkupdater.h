#pragma once

#include "component.h"

namespace tarragon
{
    class ChunkUpdater : public UpdateComponent
    {
    public:
        ChunkUpdater() = default;
        virtual ~ChunkUpdater() = default;
        ChunkUpdater(ChunkUpdater const&) = delete;
        ChunkUpdater& operator= (ChunkUpdater const&) = delete;

        virtual void update(Clock const& clock) override;
    };
}
