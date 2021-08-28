#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "glad/gl.h"
#include "component.h"
#include "chunk.h"
#include "camera.h"
#include "shader.h"

namespace tarragon
{
    struct Vertex
    {
    	glm::vec3 Position;
    	glm::vec4 Color;
    	glm::vec2 TexCoord;
    };

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

        void upload(ChunkMesher::MeshData *pdata);

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
        Shader m_shader;
        std::vector<ChunkBindingsPtr> m_bindings;

    public:
        ChunkRenderer(Camera *pcamera)
            : m_pcamera{pcamera}
        { }
        virtual ~ChunkRenderer() = default;

        ChunkRenderer(ChunkRenderer const&) = delete;
        ChunkRenderer& operator= (ChunkRenderer const&) = delete;

        void init_shader();

        virtual void update(Clock const& clock) override;
        virtual void draw() override;
    };
}
