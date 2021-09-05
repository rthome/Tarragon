#pragma once

#include <cassert>
#include <functional>
#include <queue>
#include <mutex>

#include <glm/geometric.hpp>

#include "common.h"
#include "component.h"
#include "chunk.h"
#include "camera.h"

namespace tarragon
{
	class ChunkTransfer : public UpdateComponent
	{
	private:
		struct ChunkDistance
		{
			Camera* m_pcamera;

			bool operator()(const Chunk* lhs, const Chunk* rhs) const
			{
				assert(lhs != nullptr);
				assert(rhs != nullptr);
				return glm::distance(m_pcamera->position(), glm::vec3{ lhs->center() }) > glm::distance(m_pcamera->position(), glm::vec3{ rhs->center() });
			}

			ChunkDistance(Camera* pcamera)
				: m_pcamera{ pcamera }
			{ }
		};

		std::mutex m_queue_mtx;
		std::priority_queue<Chunk*, std::vector<Chunk*>, ChunkDistance> m_load_queue;
		std::queue<Chunk*> m_finished_queue;

	public:
		explicit ChunkTransfer(Camera* pcamera)
			: m_queue_mtx{}
			, m_load_queue{ ChunkDistance{ pcamera }, {} }
			, m_finished_queue{}
		{

		}
		virtual ~ChunkTransfer() = default;

		ChunkTransfer(ChunkTransfer const&) = delete;
		ChunkTransfer& operator= (ChunkTransfer const&) = delete;

		virtual void initialize() override
		{

		}

		virtual void update(Clock const& clock) override
		{
			UNUSED_PARAM(clock);
		}

		void enqueue_to_load(Chunk* pchunk);
		void enqueue_to_render(Chunk* pchunk);

		bool dequeue_to_load(Chunk** ppchunk);
		bool dequeue_to_render(Chunk** ppchunk);
	};
}
