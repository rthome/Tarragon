#include "chunk.h"

namespace tarragon
{
    IncId<int64_t> Chunk::s_id{};

    void Chunk::fill_from(Module source)
    {
        m_has_data = true;
        for (size_t z = 0; z < Chunk::WIDTH; z++)
        {
            for (size_t y = 0; y < Chunk::WIDTH; y++)
            {
                for (size_t x = 0; x < Chunk::WIDTH; x++)
                {
                    glm::size3 index{ x, y, z };

                    auto sample_position = extents().world_position_at_index(index);
                    auto value = source(sample_position);
                    set_at(index, value);
                }
            }
        }
    }
}
