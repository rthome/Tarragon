#include "chunkupdater.h"

#include <array>
#include <vector>

#include "common.h"
#include "camera.h"

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

    ChunkMesh ChunkUpdater::generate_mesh(Chunk* chunk)
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

                            data.TexCoords.push_back({ 0.0f, 0.0f });
                            data.TexCoords.push_back({ 1.0f, 0.0f });
                            data.TexCoords.push_back({ 0.0f, 1.0f });
                            data.TexCoords.push_back({ 1.0f, 1.0f });

                            index += 4;
                        }
                    }
                }
            }
        }

        data.WorldPosition = glm::vec3{ chunk->extents().origin() };
        return data;
    }

    void ChunkUpdater::work_thread_loop()
    {
        while (true)
        {
            Chunk* pgenchunk{};
            if (m_pchunk_transfer->dequeue_to_load(&pgenchunk))
            {
                Module xdisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 0);
                Module ydisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 1);
                Module zdisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 2);
                Module source = Displace(RidgedMulti(1 / 72.0, 2.3, 14, NoiseQuality::Best, 0),
                    xdisp, ydisp, zdisp);

                pgenchunk->fill_from(source);
                auto mesh_data = generate_mesh(pgenchunk);
                pgenchunk->set_mesh(std::move(mesh_data));

                m_pchunk_transfer->enqueue_to_render(pgenchunk);

                std::this_thread::yield();
            }
            else
                std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
        }
    }

    void ChunkUpdater::update(Clock const& clock)
    {
        UNUSED_PARAM(clock);
    }
}
