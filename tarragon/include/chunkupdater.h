#pragma once

#include <array>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "common.h"
#include "component.h"
#include "chunk.h"
#include "chunktransfer.h"

#include <noise/modules.h>
using namespace tarragon::noise;

namespace tarragon
{
    class ChunkUpdater : public UpdateComponent
    {
    private:
        ChunkTransfer* m_pchunk_transfer;

        double m_air_threshold{ 0.2 };

    public:
        ChunkUpdater(ChunkTransfer* ptransfer)
            : m_pchunk_transfer{ ptransfer }
        {

        }
        virtual ~ChunkUpdater() = default;
        
        ChunkUpdater(ChunkUpdater const&) = delete;
        ChunkUpdater& operator= (ChunkUpdater const&) = delete;

        virtual void initialize() override
        {
            
        }

        virtual void update(Clock const& clock) override;

        ChunkMesh generate_mesh(Chunk* chunk);
    };
}
