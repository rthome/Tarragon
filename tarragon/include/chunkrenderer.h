#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "glad/gl.h"
#include "component.h"
#include "chunk.h"
#include "camera.h"
#include "shader.h"
#include "chunktransfer.h"

namespace tarragon
{
    class ChunkBindings
    {
    private:
        Chunk::Extents m_chunk_extents;

        GLuint m_vao{};
        GLuint m_position_buffer{};
        GLuint m_normal_buffer{};
        GLuint m_texcoord_buffer{};
        GLuint m_index_buffer{};
        GLsizei m_index_count{};
        glm::mat4 m_model{};

    //public:
    //    GLuint m_normal_vao{};
    //    GLuint m_normalline_buffer{};
    //    GLsizei m_normalline_count{};

    public:
        ChunkBindings(Chunk::Extents const& chunk_extents);
        ~ChunkBindings();

        ChunkBindings(ChunkBindings const&) = delete;
        ChunkBindings& operator= (ChunkBindings const&) = delete;

        void upload(const ChunkMesh *pdata);

        Chunk::Extents const& chunk_extents() const noexcept { return m_chunk_extents; }

        GLuint vao() const noexcept { return m_vao; }
        GLsizei index_count() const noexcept { return m_index_count; }
        glm::mat4 const& model() const noexcept { return m_model; }
    };
    using ChunkBindingsPtr = std::shared_ptr<ChunkBindings>;


    class ChunkRenderer : public UpdateComponent, public DrawComponent
    {
    private:
        Camera* m_pcamera;
        ChunkTransfer* m_pchunk_transfer;

        Shader m_shader;
        //Shader m_normal_shader;
        std::vector<ChunkBindingsPtr> m_bindings;

        GLuint m_rock_texture{};

    public:
        ChunkRenderer(Camera *pcamera, ChunkTransfer* ptransfer)
            : m_pcamera{ pcamera }
            , m_pchunk_transfer{ ptransfer }
        { }
        virtual ~ChunkRenderer() = default;

        ChunkRenderer(ChunkRenderer const&) = delete;
        ChunkRenderer& operator= (ChunkRenderer const&) = delete;

        virtual void initialize() override;

        virtual void update(Clock const& clock) override;
        virtual void draw() override;
    };
}
