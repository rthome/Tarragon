#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/std_based_type.hpp>

#include "noise/modules.h"

using namespace tarragon::noise;

namespace tarragon
{
    template <size_t width>
    class ChunkExtents final
    {
    public:
        static constexpr double BLOCKSIZE = 1.0;

    private:
        // bottom front left, where block {0,0,0} is located
        glm::dvec3 m_origin;

    public:
        ChunkExtents(glm::dvec3 const& origin)
            : m_origin{ origin }
        {
        }

        // Offsets to go from origin to the center of each face (or center)
        glm::dvec3 left_offset() const { return glm::dvec3{ 0.0, 0.5, 0.5 } * BLOCKSIZE * width; }
        glm::dvec3 right_offset() const { return glm::dvec3{ 1.0, 0.5, 0.5 } * BLOCKSIZE * width; }
        glm::dvec3 front_offset() const { return glm::dvec3{ 0.5, 0.0, 0.5 } * BLOCKSIZE * width; }
        glm::dvec3 back_offset() const { return glm::dvec3{ 0.5, 1.0, 0.5 } * BLOCKSIZE * width; }
        glm::dvec3 bottom_offset() const { return glm::dvec3{ 0.5, 0.5, 0.0 } * BLOCKSIZE * width; }
        glm::dvec3 top_offset() const { return glm::dvec3{ 0.5, 0.5, 1.0 } * BLOCKSIZE * width; }
        glm::dvec3 center_offset() const { return glm::dvec3{ 0.5, 0.5, 0.5 } * BLOCKSIZE * width; }

        glm::dvec3 const& origin() const { return m_origin; }
        glm::dvec3 center() const { return m_origin + center_offset(); }

        // Absolute positions of the centers each of the sides
        glm::dvec3 left_face() const { return m_origin + left_offset(); }
        glm::dvec3 right_face() const { return m_origin + right_offset(); }
        glm::dvec3 front_face() const { return m_origin + front_offset(); }
        glm::dvec3 back_face() const { return m_origin + back_offset(); }
        glm::dvec3 bottom_face() const { return m_origin + bottom_offset(); }
        glm::dvec3 top_face() const { return m_origin + top_offset(); }

        glm::dvec3 world_position_at_index(glm::size3 const& index) const
        {
            return origin() + (glm::dvec3{ index } * BLOCKSIZE);
        }
    };

    class Chunk final
    {
    public:
        static constexpr size_t WIDTH = 16;

        using Extents = ChunkExtents<WIDTH>;
        using DataArray = std::array<double, WIDTH * WIDTH * WIDTH>;

    private:
        Extents m_extents;
        std::unique_ptr<DataArray> m_pdata;

    public:
        Chunk(glm::dvec3 center_position)
            : m_extents{ center_position }
            , m_pdata{ std::make_unique<DataArray>() }
        { }

        Extents const& extents() const { return m_extents; }
        const DataArray* data() const { return m_pdata.get(); }

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
            return m_pdata->at(fidx);
        }

        void set_value(glm::size3 const& idx, double value)
        {
            auto fidx = index_for(idx);
            m_pdata->at(fidx) = value;
        }

        void fill_from(Module source);
    };

    class ChunkMesher final
    {
    public:
        struct MeshData
        {
            glm::vec3 WorldPosition;
            std::vector<glm::vec3> Positions;
            std::vector<glm::vec4> Colors;
            std::vector<glm::vec3> Normals;
            std::vector<uint32_t> Indices;
        };

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

    private:
        double m_air_threshold;

    public:
        ChunkMesher(double air_threshold)
            : m_air_threshold{air_threshold}
        { }

        MeshData mesh(Chunk *pchunk);
    };
}
