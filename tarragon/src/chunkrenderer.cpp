#include "chunkrenderer.h"

#include <glm/ext/matrix_transform.hpp>

#include "glad/gl.h"
#include <common.h>
#include "shadersource.h"

#include <noise/modules.h>
using namespace tarragon::noise;

namespace tarragon
{
    ChunkBindings::ChunkBindings()
    {
        glCreateVertexArrays(1, &m_vao);
        glCreateBuffers(1, &m_vertex_buffer);
	    glCreateBuffers(1, &m_index_buffer);
    }

    ChunkBindings::~ChunkBindings()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
    }

    void ChunkBindings::upload(ChunkMesher::MeshData *pdata)
    {
        std::vector<Vertex> vertices{};
	    for (size_t i = 0; i < pdata->Positions.size(); i++)
	    	vertices.push_back({pdata->Positions.at(i), pdata->Colors.at(i), {}});
            
        m_index_count = static_cast<GLsizei>(pdata->Indices.size());
        m_model = glm::translate(glm::identity<glm::mat4>(), pdata->WorldPosition);

	    glNamedBufferStorage(m_vertex_buffer, sizeof(Vertex) * vertices.size(), vertices.data(), 0);
	    glNamedBufferStorage(m_index_buffer, sizeof(uint32_t) * pdata->Indices.size(), pdata->Indices.data(), 0);

	    glVertexArrayElementBuffer(m_vao, m_index_buffer);

	    glEnableVertexArrayAttrib(m_vao, 0);
	    glVertexArrayVertexBuffer(m_vao, 0, m_vertex_buffer, offsetof(Vertex, Position), sizeof(Vertex));
	    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

	    glEnableVertexArrayAttrib(m_vao, 1);
	    glVertexArrayVertexBuffer(m_vao, 1, m_vertex_buffer, offsetof(Vertex, Color), sizeof(Vertex));
	    glVertexArrayAttribFormat(m_vao, 1, 4, GL_FLOAT, GL_FALSE, 0);

	    glVertexArrayAttribBinding(m_vao, 0, 0);
	    glVertexArrayAttribBinding(m_vao, 1, 1);
    }

    void ChunkRenderer::initialize()
    {
        m_shader.add_shader(ShaderType::Vertex, VERTEX_SOURCE);
	    m_shader.add_shader(ShaderType::Fragment, FRAGMENT_SOURCE);
	    m_shader.link();

        Module source = Cell(CellType::Quadratic, 0.5, 0.15, true, 4.0, 12354);

	    Chunk chunk{glm::dvec3{1.0, 2.0, 3.0}};
	    chunk.fill_from(source);

	    ChunkMesher mesher{0.5};
	    ChunkMesher::MeshData mesh_data = mesher.mesh(chunk.data());

        ChunkBindingsPtr pbinding = std::make_shared<ChunkBindings>();
        pbinding->upload(&mesh_data);
        m_bindings.push_back(pbinding); 
    }

    void ChunkRenderer::update(Clock const& clock)
    {
        UNUSED_PARAM(clock);
    }

    void ChunkRenderer::draw()
    {  
        m_shader.use();
		m_shader["view"].write(m_pcamera->view());
		m_shader["projection"].write(m_pcamera->projection());

        for (auto pbindings : m_bindings)
        {
            m_shader["model"].write(pbindings->model());

            glBindVertexArray(pbindings->vao());
			glDrawElements(GL_TRIANGLES, pbindings->index_count(), GL_UNSIGNED_INT, nullptr);
        }
    }
}