#pragma once

#include <cstdint>
#include <array>
#include <memory>

namespace tarragon::noise
{
    class Chunk
    {
    public:
        // Total chunk size is width*width*height
        static constexpr size_t WIDTH = 32;
        static constexpr size_t HEIGHT = 16;

        using DataArray = std::array<double, WIDTH * WIDTH * HEIGHT>;

    private:
        DataArray m_data;

    public:
        DataArray const& data() const { return m_data; }
        size_t data_size() const { return m_data.size(); }

        auto begin() { return std::begin(m_data); }
        auto end() { return std::end(m_data); }
        auto cbegin() const { return std::cbegin(m_data); }
        auto cend() const { return std::cend(m_data); }
    };

    using ChunkPtr = std::shared_ptr<Chunk>; 
}
