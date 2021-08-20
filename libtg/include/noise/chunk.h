#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <memory>

#include <glm/gtx/std_based_type.hpp>

namespace tarragon::noise
{
    class Chunk
    {
    public:
        // Total chunk size is width*width*width
        static constexpr size_t WIDTH = 16;

        using DataArray = std::array<double, WIDTH * WIDTH * WIDTH>;

    private:
        DataArray m_data;

    public:
        DataArray const& data() const { return m_data; }
        size_t data_size() const { return m_data.size(); }

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

    using ChunkPtr = std::shared_ptr<Chunk>; 
}
