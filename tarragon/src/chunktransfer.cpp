#include "chunktransfer.h"

#include <glm/geometric.hpp>

namespace tarragon
{
	void ChunkTransfer::update(Clock const& clock)
	{
		UNUSED_PARAM(clock);

		// Queue new chunks for loading
		auto chunk_indices = m_pchunk_cache->chunk_indices_around(m_pcamera->position(), ChunkLoadDistance);
		for (auto& index : chunk_indices)
		{
			auto pchunk = m_pchunk_cache->get_chunk_at(index);
			if (pchunk->state() == ChunkState::Created)
				enqueue_to_load(pchunk);
		}

		// Queue old chunks for unloading
		for (auto pchunk : m_rendering_chunks)
		{
			if (glm::distance(pchunk->center(), glm::dvec3{ m_pcamera->position() }) > ChunkUnloadThreshold && pchunk->state() == ChunkState::Ready)
				enqueue_to_unload(pchunk);
		}
	}

	void ChunkTransfer::enqueue_to_load(Chunk* pchunk)
	{
		std::lock_guard g{ m_queue_mtx };

		pchunk->state() = ChunkState::Loading;
		m_load_queue.push(pchunk);
	}

	bool ChunkTransfer::dequeue_to_load(Chunk** ppchunk)
	{
		*ppchunk = nullptr;

		std::lock_guard g{ m_queue_mtx };

		if (m_load_queue.empty())
			return false;
		*ppchunk = m_load_queue.top();
		m_load_queue.pop();
		return true;
	}

	void ChunkTransfer::enqueue_to_render(Chunk* pchunk)
	{
		std::lock_guard g{ m_queue_mtx };

		pchunk->state() = ChunkState::Ready;
		m_finished_queue.push(std::move(pchunk));
	}

	bool ChunkTransfer::dequeue_to_render(Chunk** ppchunk)
	{
		*ppchunk = nullptr;

		std::lock_guard g{ m_queue_mtx };

		if (m_finished_queue.empty())
			return false;
		*ppchunk = m_finished_queue.front();
		m_finished_queue.pop();

		m_rendering_chunks.insert(*ppchunk);

		return true;
	}

	void ChunkTransfer::enqueue_to_unload(Chunk* pchunk)
	{
		std::lock_guard g{ m_queue_mtx };

		pchunk->state() = ChunkState::Unloading;
		m_unload_queue.push(std::move(pchunk));
	}

	bool ChunkTransfer::dequeue_to_unload(Chunk** ppchunk)
	{
		*ppchunk = nullptr;

		std::lock_guard g{ m_queue_mtx };

		if (m_unload_queue.empty())
			return false;
		*ppchunk = m_unload_queue.front();
		m_unload_queue.pop();

		m_rendering_chunks.erase(*ppchunk);

		return true;
	}
}
