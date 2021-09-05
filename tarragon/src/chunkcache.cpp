#include "chunkcache.h"

#include <algorithm>

#include <glm/geometric.hpp>

namespace tarragon
{
	ChunkIndex ChunkCache::get_chunk_index(glm::dvec3 const& world_position)
	{
		auto index_position = world_position / Chunk::Extents::CHUNK_WIDTH;
		return glm::i64vec3{ glm::floor(index_position) };
	}

	glm::dvec3 ChunkCache::get_chunk_origin(ChunkIndex const& chunk_index)
	{
		return glm::dvec3{ chunk_index } * Chunk::Extents::CHUNK_WIDTH;
	}

	glm::dvec3 ChunkCache::get_chunk_center(ChunkIndex const& chunk_index)
	{
		return get_chunk_origin(chunk_index) + Chunk::Extents::center_offset();
	}

	int64_t ChunkCache::get_chunk_index_hash(ChunkIndex const& chunk_index)
	{
		glm::u64vec3 ui{ chunk_index };
		return static_cast<int64_t>((ui.x * P0 + ui.y) * P1 + ui.z);
	}

	Chunk* ChunkCache::get_chunk_at(glm::dvec3 const& world_pos)
	{
		auto chunk_index = get_chunk_index(world_pos);
		return get_chunk_at(chunk_index);
	}

	Chunk* ChunkCache::get_chunk_at(ChunkIndex const& chunk_index)
	{
		auto chunk_hash = get_chunk_index_hash(chunk_index);

		auto [first, last] = m_chunks.equal_range(chunk_hash);
		for (; first != last; first++)
		{
			auto candidate_index = get_chunk_index(first->second->center());
			if (chunk_index == candidate_index)
				return first->second.get();
		}

		// no chunk found, create it
		auto chunk_origin = get_chunk_origin(chunk_index);
		auto it = m_chunks.emplace(chunk_hash, std::make_unique<Chunk>(chunk_origin, chunk_index));
		return it->second.get();
	}

	std::vector<ChunkIndex> ChunkCache::chunk_indices_around(glm::dvec3 const& world_pos, double max_distance)
	{
		assert(max_distance > 0.0);

		const auto chunk_index = get_chunk_index(world_pos);
		const auto max_index_dist = static_cast<int64_t>(glm::ceil(max_distance / Chunk::Extents::CHUNK_WIDTH));

		std::vector<ChunkIndex> collected_indices{};
		for (int64_t z = -max_index_dist; z < max_index_dist; ++z)
		{
			for (int64_t y = -max_index_dist; y < max_index_dist; ++y)
			{
				for (int64_t x = -max_index_dist; x < max_index_dist; ++x)
				{
					auto index_candidate = chunk_index + ChunkIndex{ x, y, z };
					if (glm::distance(world_pos, get_chunk_center(chunk_index)) < max_distance)
						collected_indices.push_back(std::move(index_candidate));
				}
			}
		}

		std::sort(std::begin(collected_indices), std::end(collected_indices),
			[&world_pos](ChunkIndex& a, ChunkIndex& b) { return glm::distance(get_chunk_center(a), world_pos) < glm::distance(get_chunk_center(b), world_pos); });
		return collected_indices;
	}
}
