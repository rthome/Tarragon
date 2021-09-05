#pragma once

#include <thread>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "common.h"
#include "component.h"
#include "chunk.h"
#include "chunktransfer.h"

namespace tarragon
{
    class ChunkUpdater : public UpdateComponent
    {
    private:
        ChunkTransfer* m_pchunk_transfer;

        std::jthread m_work_thread_0;
        std::jthread m_work_thread_1;
        double m_air_threshold{ 0.1 };

        ChunkMesh generate_mesh(Chunk* chunk);

        void work_thread_loop();

    public:
        ChunkUpdater(ChunkTransfer* ptransfer)
            : m_pchunk_transfer{ ptransfer }
            , m_work_thread_0{ &ChunkUpdater::work_thread_loop, this }
            , m_work_thread_1{ &ChunkUpdater::work_thread_loop, this }
        {

        }
        virtual ~ChunkUpdater() = default;
        
        ChunkUpdater(ChunkUpdater const&) = delete;
        ChunkUpdater& operator= (ChunkUpdater const&) = delete;

        virtual void initialize() override
        {
            
        }

        virtual void update(Clock const& clock) override;
    };
}
