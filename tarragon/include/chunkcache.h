#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "chunk.h"

namespace tarragon
{
	using ChunkIndex = glm::i64vec3;

	class ChunkCache
	{
	public:
		// gets the index of the chunk, ie the xth/yth/zth chunk on each axis
		static ChunkIndex get_chunk_index(glm::dvec3 const& world_position);

		// calculates the origin position (in world coordinates) from the chunk index
		static glm::dvec3 get_chunk_origin(ChunkIndex const& chunk_index);

		// calculates the center position (in world coordinates) from the chunk index
		static glm::dvec3 get_chunk_center(ChunkIndex const& chunk_index);

	private:
		static constexpr uint64_t P0 = 1050112070355889ui64;
		static constexpr uint64_t P1 = 2456099197ui64;

		// calculates a primitive hash from the chunk index to improve lookup
		static int64_t get_chunk_index_hash(ChunkIndex const& origin_position);

		std::unordered_multimap<int64_t, std::unique_ptr<Chunk>> m_chunks{};

	public:
		Chunk* get_chunk_at(glm::dvec3 const& world_pos);
		Chunk* get_chunk_at(ChunkIndex const& chunk_index);

		std::vector<ChunkIndex> chunk_indices_around(glm::dvec3 const& world_pos, double max_distance);
	};
}
