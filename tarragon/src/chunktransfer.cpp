#include "chunktransfer.h"

namespace tarragon
{
	void ChunkTransfer::enqueue_to_load(Chunk* pchunk)
	{
		std::lock_guard g{ m_queue_mtx };

		pchunk->state() = ChunkState::Loading;
		m_load_queue.push(std::move(pchunk));
	}

	void ChunkTransfer::enqueue_to_render(Chunk* pchunk)
	{
		std::lock_guard g{ m_queue_mtx };

		pchunk->state() = ChunkState::Ready;
		m_finished_queue.push(std::move(pchunk));
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

	bool ChunkTransfer::dequeue_to_render(Chunk** ppchunk)
	{
		*ppchunk = nullptr;

		std::lock_guard g{ m_queue_mtx };

		if (m_finished_queue.empty())
			return false;
		*ppchunk = m_finished_queue.front();
		m_finished_queue.pop();
		return true;
	}
}
