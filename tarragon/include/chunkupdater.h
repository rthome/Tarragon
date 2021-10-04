#pragma once

#include <memory>
#include <thread>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "common.h"
#include "component.h"
#include "chunk.h"
#include "chunktransfer.h"
#include "world.h"

namespace tarragon
{
    class ChunkUpdater : public UpdateComponent
    {
    private:
        ChunkTransfer* m_pchunk_transfer;

        std::jthread m_work_thread_0;
        std::jthread m_work_thread_1;
        
        std::unique_ptr<World> m_pworld;

        ChunkMesh generate_mesh(Chunk* chunk);

        void work_thread_loop();

    public:
        ChunkUpdater(ChunkTransfer* ptransfer)
            : m_pchunk_transfer{ ptransfer }
            , m_work_thread_0{ &ChunkUpdater::work_thread_loop, this }
            , m_work_thread_1{ &ChunkUpdater::work_thread_loop, this }
            , m_pworld{ std::make_unique<World>() }
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
