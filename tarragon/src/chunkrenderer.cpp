#include "chunkrenderer.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/normal.hpp>

#include "glad/gl.h"
#include <common.h>
#include "shadersource.h"

#include <noise/modules.h>
using namespace tarragon::noise;

namespace tarragon
{
    namespace
    {
        using QuadArray = std::array<glm::ivec3, 4>;

        static constexpr std::array<glm::ivec3, 6> Neighbours6
        {
            glm::ivec3{1, 0, 0},  //right
            glm::ivec3{-1, 0, 0}, //left
            glm::ivec3{0, 1, 0},  //top
            glm::ivec3{0, -1, 0}, //bottom
            glm::ivec3{0, 0, 1},  //front
            glm::ivec3{0, 0, -1}  //back
        };

        static constexpr std::array<QuadArray, 6> NeighbourFaces
        {
            QuadArray { //right
                glm::ivec3{1, 1, 1},
                glm::ivec3{1, 1, 0},
                glm::ivec3{1, 0, 1},
                glm::ivec3{1, 0, 0},
            },
            QuadArray { //left
                glm::ivec3{0, 1, 0},
                glm::ivec3{0, 1, 1},
                glm::ivec3{0, 0, 0},
                glm::ivec3{0, 0, 1},
            },
            QuadArray { //top
                glm::ivec3{0, 1, 0},
                glm::ivec3{1, 1, 0},
                glm::ivec3{0, 1, 1},
                glm::ivec3{1, 1, 1},
            },
            QuadArray { //bottom
                glm::ivec3{1, 0, 0},
                glm::ivec3{0, 0, 0},
                glm::ivec3{1, 0, 1},
                glm::ivec3{0, 0, 1},
            },
            QuadArray { //front
                glm::ivec3{0, 1, 1},
                glm::ivec3{1, 1, 1},
                glm::ivec3{0, 0, 1},
                glm::ivec3{1, 0, 1},
            },
            QuadArray { //back
                glm::ivec3{1, 1, 0},
                glm::ivec3{0, 1, 0},
                glm::ivec3{1, 0, 0},
                glm::ivec3{0, 0, 0},
            },
        };
    }

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

    void ChunkBindings::upload(ChunkMesh *pdata)
    {
        if (pdata->Positions.size() > 0)
        {
            std::vector<ChunkVertex> vertices{};
            for (size_t i = 0; i < pdata->Positions.size(); i++)
                vertices.push_back({pdata->Positions.at(i), pdata->Colors.at(i), pdata->Normals.at(i)});

            m_index_count = static_cast<GLsizei>(pdata->Indices.size());
            m_model = glm::translate(glm::identity<glm::mat4>(), pdata->WorldPosition);

            glNamedBufferStorage(m_vertex_buffer, sizeof(ChunkVertex) * vertices.size(), vertices.data(), 0);
            glNamedBufferStorage(m_index_buffer, sizeof(uint32_t) * pdata->Indices.size(), pdata->Indices.data(), 0);
        }

        glVertexArrayElementBuffer(m_vao, m_index_buffer);

        glEnableVertexArrayAttrib(m_vao, 0);
        glVertexArrayVertexBuffer(m_vao, 0, m_vertex_buffer, offsetof(ChunkVertex, Position), sizeof(ChunkVertex));
        glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayVertexBuffer(m_vao, 1, m_vertex_buffer, offsetof(ChunkVertex, Color), sizeof(ChunkVertex));
        glVertexArrayAttribFormat(m_vao, 1, 4, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(m_vao, 2);
        glVertexArrayVertexBuffer(m_vao, 2, m_vertex_buffer, offsetof(ChunkVertex, Normal), sizeof(ChunkVertex));
        glVertexArrayAttribFormat(m_vao, 2, 3, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 1);
        glVertexArrayAttribBinding(m_vao, 2, 2);
    }

    void ChunkRenderer::initialize()
    {
        m_shader.add_shader(ShaderType::Vertex, VERTEX_SOURCE);
        m_shader.add_shader(ShaderType::Fragment, FRAGMENT_SOURCE);
        m_shader.link();

        Module xdisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 0);
        Module ydisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 1);
        Module zdisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 2);
        Module source = Displace(RidgedMulti(1 / 64.0, 2.2, 12, NoiseQuality::Best, 0),
            xdisp, ydisp, zdisp);

        m_air_threshold = 0.6;

        auto initial_chunk_indices = m_pchunk_cache->chunk_indices_around(m_pcamera->position(), 30.0);
        for (auto& index : initial_chunk_indices)
        {
            auto pchunk = m_pchunk_cache->get_chunk_at(index);
            pchunk->fill_from(source);
         
            auto mesh_data = generate_mesh(pchunk);

            ChunkBindingsPtr pbinding = std::make_shared<ChunkBindings>();
            pbinding->upload(&mesh_data);
            m_bindings.push_back(pbinding);
        }
    }

    void ChunkRenderer::update(Clock const& clock)
    {
        UNUSED_PARAM(clock);

        Module xdisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 0);
        Module ydisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 1);
        Module zdisp = Billow(1 / 16, 3, 6, 0.75, NoiseQuality::Standard, 2);
        Module source = Displace(RidgedMulti(1 / 64.0, 2.2, 12, NoiseQuality::Best, 0),
            xdisp, ydisp, zdisp);

        auto chunk_indices = m_pchunk_cache->chunk_indices_around(m_pcamera->position(), 30.0);
        for (auto& index : chunk_indices)
        {
            auto pchunk = m_pchunk_cache->get_chunk_at(index);
            if (pchunk->has_data())
                continue;
            
            pchunk->fill_from(source);
            auto mesh_data = generate_mesh(pchunk);

            ChunkBindingsPtr pbinding = std::make_shared<ChunkBindings>();
            pbinding->upload(&mesh_data);
            m_bindings.push_back(pbinding);

            break;
        }
    }

    void ChunkRenderer::draw()
    {  
        m_shader.use();
        m_shader["view"].write(m_pcamera->view());
        m_shader["projection"].write(m_pcamera->projection());

        for (auto& pbindings : m_bindings)
        {
            m_shader["model"].write(pbindings->model());

            glBindVertexArray(pbindings->vao());
            glDrawElements(GL_TRIANGLES, pbindings->index_count(), GL_UNSIGNED_INT, nullptr);
        }
    }

    ChunkMesh ChunkRenderer::generate_mesh(Chunk* chunk)
    {
        ChunkMesh data{};
        uint32_t index{};

        for (size_t z = 0; z < Chunk::WIDTH; z++)
        {
            for (size_t y = 0; y < Chunk::WIDTH; y++)
            {
                for (size_t x = 0; x < Chunk::WIDTH; x++)
                {
                    glm::size3 position{ x, y, z };

                    auto value = chunk->at(position);
                    if (value < m_air_threshold)
                        continue;

                    for (size_t i = 0; i < Neighbours6.size(); i++)
                    {
                        auto& offset = Neighbours6.at(i);
                        glm::ivec3 neighbour_pos = glm::ivec3{ position } + offset;

                        if (((neighbour_pos.x < 0 || neighbour_pos.x >= static_cast<int>(Chunk::WIDTH))
                            || (neighbour_pos.y < 0 || neighbour_pos.y >= static_cast<int>(Chunk::WIDTH))
                            || (neighbour_pos.z < 0 || neighbour_pos.z >= static_cast<int>(Chunk::WIDTH)))
                            || chunk->at(glm::size3{ neighbour_pos }) < m_air_threshold)
                        {
                            auto& quad = NeighbourFaces.at(i);
                            for (size_t j = 0; j < quad.size(); j++)
                            {
                                data.Positions.push_back(glm::vec3{ glm::ivec3{position} + quad.at(j) });
                                data.Colors.push_back(glm::vec4{ x / static_cast<float>(Chunk::WIDTH), y / static_cast<float>(Chunk::WIDTH), z / static_cast<float>(Chunk::WIDTH), 1.0f });
                            }

                            data.Indices.push_back(index + 0);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 2);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 3);
                            data.Indices.push_back(index + 2);

                            auto normal = glm::triangleNormal(data.Positions.at(index + 0ull), data.Positions.at(index + 1ull), data.Positions.at(index + 2ull));
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);

                            index += 4;
                        }
                    }
                }
            }
        }

        data.WorldPosition = glm::vec3{ chunk->extents().origin() };
        return data;
    }
}
