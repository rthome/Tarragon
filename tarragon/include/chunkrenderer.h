#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "glad/gl.h"
#include "component.h"
#include "chunk.h"
#include "camera.h"
#include "shader.h"
#include "chunkcache.h"
#include "chunktransfer.h"

namespace tarragon
{
    class ChunkBindings
    {
    private:
        GLuint m_vao{};
        GLuint m_vertex_buffer{};
        GLuint m_index_buffer{};
        GLsizei m_index_count{};
        glm::mat4 m_model{};

    public:
        ChunkBindings();
        ~ChunkBindings();

        ChunkBindings(ChunkBindings const&) = delete;
        ChunkBindings& operator= (ChunkBindings const&) = delete;

        void upload(ChunkMesh *pdata);

        GLuint vao() const { return m_vao; }
        GLuint vertex_buffer() const { return m_vertex_buffer; }
        GLuint index_buffer() const { return m_index_buffer; }
        GLsizei index_count() const { return m_index_count; }
        glm::mat4 const& model() const { return m_model; }
    };
    using ChunkBindingsPtr = std::shared_ptr<ChunkBindings>;


    class ChunkRenderer : public UpdateComponent, public DrawComponent
    {
    private:
        Camera *m_pcamera;
        ChunkCache* m_pchunk_cache;

        Shader m_shader;
        std::vector<ChunkBindingsPtr> m_bindings;
        
        double m_air_threshold{};

    public:
        ChunkRenderer(Camera *pcamera, ChunkCache* pcache)
            : m_pcamera{ pcamera }
            , m_pchunk_cache{ pcache }
        { }
        virtual ~ChunkRenderer() = default;

        ChunkRenderer(ChunkRenderer const&) = delete;
        ChunkRenderer& operator= (ChunkRenderer const&) = delete;

        virtual void initialize() override;

        virtual void update(Clock const& clock) override;
        virtual void draw() override;

        ChunkMesh generate_mesh(Chunk* chunk);
    };
}
