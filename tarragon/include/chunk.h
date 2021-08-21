#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/gtx/std_based_type.hpp>

#include "noise/modules.h"

using namespace tarragon::noise;

namespace tarragon
{
    class ChunkData final
    {
    public:
        // Total chunk size is width*width*width
        static constexpr size_t WIDTH = 16;

        using DataArray = std::array<double, WIDTH * WIDTH * WIDTH>;

    private:
        DataArray m_data;

    public:
        DataArray const& data() const { return m_data; }

        auto begin() { return std::begin(m_data); }
        auto end() { return std::end(m_data); }
        auto cbegin() const { return std::cbegin(m_data); }
        auto cend() const { return std::cend(m_data); }

        size_t index_for(glm::size3 const& idx) const
        {
            assert(idx.x < WIDTH);
            assert(idx.y < WIDTH);
            assert(idx.z < WIDTH);

            return (WIDTH * WIDTH * idx.x) + (WIDTH * idx.y) + idx.z;
        }

        double get_value(glm::size3 const& idx) const
        {
            auto fidx = index_for(idx);
            return m_data.at(fidx);
        }

        void set_value(glm::size3 const& idx, double value)
        {
            auto fidx = index_for(idx);
            m_data.at(fidx) = value;
        }
    };

    class Chunk final
    {
    private:
        glm::dvec3 m_chunk_origin;
        ChunkDataPtr m_chunk_data;

    public:
        Chunk(glm::dvec3 chunk_origin)
            : m_chunk_origin{ chunk_origin }
            , m_chunk_data{ std::make_shared<ChunkData>() }
        { }

        Chunk(glm::dvec3 chunk_origin, ChunkDataPtr chunk_data)
            : m_chunk_origin{ chunk_origin }
            , m_chunk_data{ chunk_data }
        { }

        glm::dvec3 const& origin() const { return m_chunk_origin; }
        ChunkDataPtr data() { return m_chunk_data; }
        const ChunkDataPtr data() const { return m_chunk_data; }


    };

    class ChunkMesher final
    {

    };

    using ChunkDataPtr = std::shared_ptr<ChunkData>;
    using ChunkPtr = std::shared_ptr<Chunk>;
}
