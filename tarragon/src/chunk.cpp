#include "chunk.h"

namespace tarragon
{
    void Chunk::fill_from(Module source)
    {
        for (size_t z = 0; z < Chunk::WIDTH; z++)
        {
            for (size_t y = 0; y < Chunk::WIDTH; y++)
            {
                for (size_t x = 0; x < Chunk::WIDTH; x++)
                {
                    glm::size3 index{ x, y, z };

                    auto sample_position = extents().world_position_at_index(index);
                    auto value = source(sample_position);
                    set_value(index, value);
                }
            }
        }
    }


    ChunkMesher::MeshData ChunkMesher::mesh(Chunk *pchunk)
    {
        MeshData data{};
        uint32_t index{};

        for (size_t z = 0; z < Chunk::WIDTH; z++)
        {
            for (size_t y = 0; y < Chunk::WIDTH; y++)
            {
                for (size_t x = 0; x < Chunk::WIDTH; x++)
                {
                    glm::size3 position{x, y, z};

                    auto value = pchunk->get_value(position);
                    if (value < m_air_threshold)
                        continue;

                    for (size_t i = 0; i < Neighbours6.size(); i++)
                    {
                        auto& offset = Neighbours6.at(i);
                        glm::ivec3 neighbour_pos = glm::ivec3{position} + offset;

                        if (((neighbour_pos.x < 0 || neighbour_pos.x >= static_cast<int>(Chunk::WIDTH))
                            || (neighbour_pos.y < 0 || neighbour_pos.y >= static_cast<int>(Chunk::WIDTH))
                            || (neighbour_pos.z < 0 || neighbour_pos.z >= static_cast<int>(Chunk::WIDTH)))
                            || pchunk->get_value(glm::size3{neighbour_pos}) < m_air_threshold)
                        {
                            auto& quad = NeighbourFaces.at(i);
                            for (size_t j = 0; j < quad.size(); j++)
                            {
                                data.Positions.push_back(glm::vec3{glm::ivec3{position} + quad.at(j)});
                                data.Colors.push_back(glm::vec4{x / static_cast<float>(Chunk::WIDTH), y / static_cast<float>(Chunk::WIDTH), z / static_cast<float>(Chunk::WIDTH), 1.0f});
                            }

                            data.Indices.push_back(index + 0);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 2);
                            data.Indices.push_back(index + 1);
                            data.Indices.push_back(index + 3);
                            data.Indices.push_back(index + 2);
                            index += 4;
                        }
                    }
                }
            }
        }
        
        data.WorldPosition = glm::vec3{ pchunk->extents().origin() };
        return data;
    }
}
