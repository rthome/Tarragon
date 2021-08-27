#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <memory>
#include <vector>

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

            return (WIDTH * WIDTH * idx.z) + (WIDTH * idx.y) + idx.x;
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
        std::unique_ptr<ChunkData> m_chunk_data;

    public:
        Chunk(glm::dvec3 chunk_origin)
            : m_chunk_origin{ chunk_origin }
            , m_chunk_data{ std::make_unique<ChunkData>() }
        { }

        Chunk(glm::dvec3 chunk_origin, std::unique_ptr<ChunkData>&& chunk_data)
            : m_chunk_origin{ chunk_origin }
            , m_chunk_data{ std::move(chunk_data) }
        { }

        glm::dvec3 const& origin() const { return m_chunk_origin; }

        ChunkData* data() { return m_chunk_data.get(); }
        const ChunkData* data() const { return m_chunk_data.get(); }

        void fill_from(Module source);
    };

    class ChunkMesher final
    {
    public:
        struct MeshData
        {
            std::vector<glm::vec3> Positions;
            std::vector<glm::vec4> Colors;
            std::vector<uint32_t> Indices;
        };

        using QuadArray = std::array<glm::ivec3, 4>;

        static constexpr std::array<glm::ivec3, 6> Neighbours6 {
            glm::ivec3(1, 0, 0),    //right
            glm::ivec3(-1, 0, 0),   //left
            glm::ivec3(0, 1, 0),    //top
            glm::ivec3(0, -1, 0),   //bottom
            glm::ivec3(0, 0, 1),    //front
            glm::ivec3(0, 0, -1)    //back
        };

        static constexpr std::array<QuadArray, 6> NeighbourFaces {
            QuadArray { //right
                glm::ivec3(1, 1, 1),
                glm::ivec3(1, 1, 0),
                glm::ivec3(1, 0, 1),
                glm::ivec3(1, 0, 0),
            },
            QuadArray { //left
                glm::ivec3(0, 1, 0),
                glm::ivec3(0, 1, 1),
                glm::ivec3(0, 0, 0),
                glm::ivec3(0, 0, 1),
            },
            QuadArray { //top
                glm::ivec3(0, 1, 0),
                glm::ivec3(1, 1, 0),
                glm::ivec3(0, 1, 1),
                glm::ivec3(1, 1, 1),
            },
            QuadArray { //bottom
                glm::ivec3(1, 0, 0),
                glm::ivec3(0, 0, 0),
                glm::ivec3(1, 0, 1),
                glm::ivec3(0, 0, 1),
            },
            QuadArray { //front
                glm::ivec3(0, 1, 1), 
                glm::ivec3(1, 1, 1),
                glm::ivec3(0, 0, 1),
                glm::ivec3(1, 0, 1),
            },
            QuadArray { //back
                glm::ivec3(1, 1, 0),
                glm::ivec3(0, 1, 0),
                glm::ivec3(1, 0, 0),
                glm::ivec3(0, 0, 0),
            },
        };

    private:
        double m_air_threshold;

    public:
        ChunkMesher(double air_threshold)
            : m_air_threshold{air_threshold}
        { }

        MeshData mesh(ChunkData *pchunk);
    };
}
