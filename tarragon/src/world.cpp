#include "world.h"

namespace tarragon
{
    Block World::map_value(double value)
    {
        if (value > m_air_threshold)
            return Block{ BlockType::Rock };
        else
            return Block{ BlockType::Air };
    }

    World::World()
    {
        Module xdisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 0);
        Module ydisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 1);
        Module zdisp = Billow(1 / 15, 3, 8, 0.5, NoiseQuality::Standard, 2);
        m_source = Displace(RidgedMulti(1 / 72.0, 2.3, 14, NoiseQuality::Best, 0),
            xdisp, ydisp, zdisp);
    }

    void World::generate_data(Chunk* pchunk)
    {
        for (size_t z = 0; z < Chunk::WIDTH; z++)
        {
            for (size_t y = 0; y < Chunk::WIDTH; y++)
            {
                for (size_t x = 0; x < Chunk::WIDTH; x++)
                {
                    glm::size3 index{ x, y, z };

                    auto sample_position = pchunk->extents().world_position_at_index(index);
                    auto value = m_source(sample_position);

                    Block block = map_value(value);
                    pchunk->set_at(index, block);
                }
            }
        }
    }
}
