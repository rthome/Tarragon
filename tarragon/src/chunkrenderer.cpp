#include "chunkrenderer.h"

#include <glm/ext/matrix_transform.hpp>

#include "glad/gl.h"
#include "stb/stb_image.h"
#include <common.h>

#include <noise/modules.h>
using namespace tarragon::noise;

namespace tarragon
{
    namespace
    {
        using QuadArray = std::array<glm::ivec3, 4>;

        static constexpr std::array<glm::vec3, 6> Normals6
        {
             Camera::RIGHT,   //right
            -Camera::RIGHT,   //left
             Camera::UP,      //top
            -Camera::UP,      //bottom
            -Camera::FORWARD, //front
             Camera::FORWARD, //back
        };

        static constexpr std::array<glm::ivec3, 6> Neighbours6
        {
            glm::ivec3{ 1, 0, 0 },  //right
            glm::ivec3{ -1, 0, 0 }, //left
            glm::ivec3{ 0, 1, 0 },  //top
            glm::ivec3{ 0, -1, 0 }, //bottom
            glm::ivec3{ 0, 0, 1 },  //front
            glm::ivec3{ 0, 0, -1 }, //back
        };

        static constexpr std::array<QuadArray, 6> NeighbourFaces
        {
            QuadArray //right
            {
                glm::ivec3{ 1, 1, 1 },
                glm::ivec3{ 1, 1, 0 },
                glm::ivec3{ 1, 0, 1 },
                glm::ivec3{ 1, 0, 0 },
            },
            QuadArray //left
            {
                glm::ivec3{ 0, 1, 0 },
                glm::ivec3{ 0, 1, 1 },
                glm::ivec3{ 0, 0, 0 },
                glm::ivec3{ 0, 0, 1 },
            },
            QuadArray //top
            {
                glm::ivec3{ 0, 1, 0 },
                glm::ivec3{ 1, 1, 0 },
                glm::ivec3{ 0, 1, 1 },
                glm::ivec3{ 1, 1, 1 },
            },
            QuadArray //bottom
            {
                glm::ivec3{ 1, 0, 0 },
                glm::ivec3{ 0, 0, 0 },
                glm::ivec3{ 1, 0, 1 },
                glm::ivec3{ 0, 0, 1 },
            },
            QuadArray //front
            {
                glm::ivec3{ 0, 1, 1 },
                glm::ivec3{ 1, 1, 1 },
                glm::ivec3{ 0, 0, 1 },
                glm::ivec3{ 1, 0, 1 },
            },
            QuadArray //back
            {
                glm::ivec3{ 1, 1, 0 },
                glm::ivec3{ 0, 1, 0 },
                glm::ivec3{ 1, 0, 0 },
                glm::ivec3{ 0, 0, 0 },
            },
        };
    }

    ChunkBindings::ChunkBindings(Chunk::Extents const& chunk_extents)
        : m_chunk_extents{ chunk_extents }
    {
        glCreateVertexArrays(1, &m_vao);
        glCreateBuffers(1, &m_position_buffer);
        glCreateBuffers(1, &m_normal_buffer);
        glCreateBuffers(1, &m_texcoord_buffer);
        glCreateBuffers(1, &m_index_buffer);

        //glCreateVertexArrays(1, &m_normal_vao);
        //glCreateBuffers(1, &m_normalline_buffer);
    }

    ChunkBindings::~ChunkBindings()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_position_buffer);
        glDeleteBuffers(1, &m_normal_buffer);
        glDeleteBuffers(1, &m_texcoord_buffer);
        glDeleteBuffers(1, &m_index_buffer);

        //glDeleteVertexArrays(1, &m_normal_vao);
        //glDeleteBuffers(1, &m_normalline_buffer);
    }

    void ChunkBindings::upload(ChunkMesh *pdata)
    {
        m_model = glm::translate(glm::identity<glm::mat4>(), pdata->WorldPosition);

        if (pdata->Positions.size() > 0)
        {
            glNamedBufferStorage(m_position_buffer, sizeof(glm::vec3) * pdata->Positions.size(), pdata->Positions.data(), 0);
            glNamedBufferStorage(m_normal_buffer, sizeof(glm::vec3) * pdata->Normals.size(), pdata->Normals.data(), 0);
            glNamedBufferStorage(m_texcoord_buffer, sizeof(glm::vec2) * pdata->TexCoords.size(), pdata->TexCoords.data(), 0);

            m_index_count = static_cast<GLsizei>(pdata->Indices.size());
            glNamedBufferStorage(m_index_buffer, sizeof(uint32_t) * pdata->Indices.size(), pdata->Indices.data(), 0);
        }

        glVertexArrayElementBuffer(m_vao, m_index_buffer);

        glEnableVertexArrayAttrib(m_vao, 0);
        glVertexArrayVertexBuffer(m_vao, 0, m_position_buffer, 0, sizeof(glm::vec3));
        glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayVertexBuffer(m_vao, 1, m_normal_buffer, 0, sizeof(glm::vec3));
        glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(m_vao, 2);
        glVertexArrayVertexBuffer(m_vao, 2, m_texcoord_buffer, 0, sizeof(glm::vec2));
        glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, 0);

        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 1);
        glVertexArrayAttribBinding(m_vao, 2, 2);

        //if (pdata->Positions.size() > 0)
        //{
        //    std::vector<glm::vec3> normallines;
        //    for (auto i = 0; i < pdata->Positions.size(); i++) {
        //        //retrieving the normal associated with this vertex
        //        auto const& n = pdata->Normals[i];

        //        //retrieving the vertex itself, it'll be the first point of our line
        //        auto const& v1 = pdata->Positions[i];

        //        const auto normal_length = 0.5f;
        //        //second point of our line representing the normal direction
        //        auto v2 = v1 + n * normal_length;

        //        normallines.push_back(v1);
        //        normallines.push_back(v2);
        //    }

        //    glNamedBufferStorage(m_normalline_buffer, sizeof(glm::vec3) * normallines.size(), normallines.data(), 0);
        //    m_normalline_count = normallines.size();
        //}

        //glEnableVertexArrayAttrib(m_normal_vao, 0);
        //glVertexArrayVertexBuffer(m_normal_vao, 0, m_normalline_buffer, 0, sizeof(glm::vec3));
        //glVertexArrayAttribFormat(m_normal_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

        //glVertexArrayAttribBinding(m_normal_vao, 0, 0);
    }

    void ChunkRenderer::initialize()
    {
        m_shader.add_shader_from_file(ShaderType::Vertex, { "shaders/chunk.vs" });
        m_shader.add_shader_from_file(ShaderType::Fragment, { "shaders/chunk.fs" });
        m_shader.link();

        //m_normal_shader.add_shader_from_file(ShaderType::Vertex, { "shaders/chunk_normals.vs" });
        //m_normal_shader.add_shader_from_file(ShaderType::Fragment, { "shaders/chunk_normals.fs" });
        //m_normal_shader.link();

        m_air_threshold = 0.0;

        int width{}, height{}, channels{};
        unsigned char *pimage_data = stbi_load("res/rock-diffuse.png", &width, &height, &channels, 4);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_rock_texture);
        glTextureParameteri(m_rock_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_rock_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_rock_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_rock_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureStorage2D(m_rock_texture, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(m_rock_texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pimage_data);
        glGenerateTextureMipmap(m_rock_texture);

        stbi_image_free(pimage_data);
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

            ChunkBindingsPtr pbinding = std::make_shared<ChunkBindings>(pchunk->extents());
            pbinding->upload(&mesh_data);
            m_bindings.push_back(pbinding);

            break;
        }
    }

    void ChunkRenderer::draw()
    {  
        m_shader.use();
        m_shader["View"].write(m_pcamera->view());
        m_shader["Projection"].write(m_pcamera->projection());
        
        m_shader["L.position"].write(m_pcamera->position());
        m_shader["L.ambient"].write(glm::vec3{ 0.3f, 0.3f, 0.3f });
        m_shader["L.diffuse"].write(glm::vec3{ 0.8f, 0.8f, 0.8f });
        m_shader["L.specular"].write(glm::vec3{ 1.0f, 1.0f, 1.0f });
        m_shader["L.constant"].write(1.0f);
        m_shader["L.linear"].write(0.045f);
        m_shader["L.quadratic"].write(0.0075f);
        
        glBindTextureUnit(0, m_rock_texture);
        m_shader["TexDiffuse"].write(0);

        for (auto& pbindings : m_bindings)
        {
            m_shader["Model"].write(pbindings->model());

            glBindVertexArray(pbindings->vao());
            glDrawElements(GL_TRIANGLES, pbindings->index_count(), GL_UNSIGNED_INT, nullptr);
        }

        //m_normal_shader.use();
        //m_normal_shader["View"].write(m_pcamera->view());
        //m_normal_shader["Projection"].write(m_pcamera->projection());

        //for (auto& pbindings : m_bindings)
        //{
        //    m_normal_shader["Model"].write(pbindings->model());

        //    glBindVertexArray(pbindings->m_normal_vao);
        //    glDrawArrays(GL_LINES, 0, pbindings->m_normalline_count);
        //}
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
                            }

                            data.Indices.push_back(index + 0);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 2);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 3);
                            data.Indices.push_back(index + 2);

                            auto const& normal = Normals6.at(i);
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);
                            data.Normals.push_back(normal);

                            data.TexCoords.push_back({0.0f, 0.0f});
                            data.TexCoords.push_back({1.0f, 0.0f});
                            data.TexCoords.push_back({0.0f, 1.0f});
                            data.TexCoords.push_back({1.0f, 1.0f});

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
