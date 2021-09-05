#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtx/std_based_type.hpp>

#include "common.h"
#include "noise/modules.h"

using namespace tarragon::noise;

namespace tarragon
{
    enum class ChunkState
    {
        Created, // Created, no data yet
        Loading, // Waiting to generate and mesh
        Ready, // Ready to render
    };

    struct ChunkMesh
    {
        glm::vec3 WorldPosition;
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec2> TexCoords;
        std::vector<uint32_t> Indices;
    };

    template <size_t Width, double BlockSize>
    class ChunkExtents final
    {
        static_assert(Width > 0, "Chunk width can't be 0.");
        static_assert(BlockSize > 0.0, "Chunk block size can't be 0 or less.");

    public:
        static constexpr double CHUNK_WIDTH = Width * BlockSize;

    private:
        // bottom front left, where block {0,0,0} is located
        glm::dvec3 m_origin;

    public:
        explicit constexpr ChunkExtents(glm::dvec3 const& origin)
            : m_origin{ origin }
        {
        }

        // Offsets to go from origin to the center of each face (or center)
        static constexpr glm::dvec3 left_offset() { return glm::dvec3{ 0.0, 0.5, 0.5 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 right_offset() { return glm::dvec3{ 1.0, 0.5, 0.5 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 front_offset() { return glm::dvec3{ 0.5, 0.0, 0.5 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 back_offset() { return glm::dvec3{ 0.5, 1.0, 0.5 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 bottom_offset() { return glm::dvec3{ 0.5, 0.5, 0.0 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 top_offset() { return glm::dvec3{ 0.5, 0.5, 1.0 } * CHUNK_WIDTH; }
        static constexpr glm::dvec3 center_offset() { return glm::dvec3{ 0.5, 0.5, 0.5 } * CHUNK_WIDTH; }

        constexpr glm::dvec3 origin() const { return m_origin; }
        constexpr glm::dvec3 center() const { return m_origin + center_offset(); }

        // Absolute positions of the centers each of the sides
        constexpr glm::dvec3 left_face() const { return m_origin + left_offset(); }
        constexpr glm::dvec3 right_face() const { return m_origin + right_offset(); }
        constexpr glm::dvec3 front_face() const { return m_origin + front_offset(); }
        constexpr glm::dvec3 back_face() const { return m_origin + back_offset(); }
        constexpr glm::dvec3 bottom_face() const { return m_origin + bottom_offset(); }
        constexpr glm::dvec3 top_face() const { return m_origin + top_offset(); }

        constexpr glm::dvec3 world_position_at_index(glm::size3 const& index) const
        {
            return m_origin + (glm::dvec3{ index } * BlockSize);
        }
    };

    using ChunkIndex = glm::i64vec3;

    class Chunk final
    {
    public:
        static constexpr size_t WIDTH = 16;

        using Extents = ChunkExtents<WIDTH, 1.0>;
        using DataArray = std::array<double, WIDTH * WIDTH * WIDTH>;

        static constexpr size_t index_for(glm::size3 const& position) noexcept
        {
            assert(position.x < WIDTH);
            assert(position.y < WIDTH);
            assert(position.z < WIDTH);

            return (WIDTH * WIDTH * position.z) + (WIDTH * position.y) + position.x;
        }

    private:
        ChunkIndex m_chunk_index;
        Extents m_extents;

        ChunkState m_state;
        std::unique_ptr<DataArray> m_pdata;
        std::unique_ptr<ChunkMesh> m_pmesh;

    public:
        Chunk(glm::dvec3 const& world_origin, ChunkIndex chunk_index)
            : m_extents{ world_origin }
            , m_chunk_index{ chunk_index }
            , m_state{ ChunkState::Created }
            , m_pdata{ std::make_unique<DataArray>() }
            , m_pmesh{}
        { }

        constexpr ChunkIndex const& chunk_index() const noexcept { return m_chunk_index; }

        constexpr Extents const& extents() const noexcept { return m_extents; }
        constexpr glm::dvec3 center() const noexcept { return m_extents.center(); }
        constexpr glm::dvec3 origin() const noexcept { return m_extents.origin(); }

        constexpr ChunkState const& state() const noexcept { return m_state; }
        constexpr ChunkState& state() noexcept { return m_state; }

        const DataArray* data() const noexcept { return m_pdata.get(); }
        const ChunkMesh* mesh() const noexcept { return m_pmesh.get(); }
        
        void set_mesh(ChunkMesh&& mesh)
        {
            m_pmesh = std::make_unique<ChunkMesh>(mesh);
        }

        constexpr double at(glm::size3 const& pos) const
        {
            auto index = Chunk::index_for(pos);
            return m_pdata->at(index);
        }

        void set_at(glm::size3 const& pos, double value)
        {
            auto index = Chunk::index_for(pos);
            m_pdata->at(index) = value;
        }

        void fill_from(Module source);
    };
}
