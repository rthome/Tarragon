#pragma once

#include <cassert>
#include <functional>
#include <queue>
#include <mutex>
#include <set>

#include <glm/geometric.hpp>

#include "common.h"
#include "component.h"
#include "chunk.h"
#include "camera.h"
#include "chunkcache.h"

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

		static constexpr double ChunkLoadDistance = 30.0;
		static constexpr double ChunkUnloadThreshold = 60.0;

		Camera* m_pcamera;
		ChunkCache* m_pchunk_cache;

		std::mutex m_queue_mtx;
		std::priority_queue<Chunk*, std::vector<Chunk*>, ChunkDistance> m_load_queue;
		std::queue<Chunk*> m_finished_queue;
		std::queue<Chunk*> m_unload_queue;

		std::set<Chunk*> m_rendering_chunks;

	public:
		ChunkTransfer(Camera* pcamera, ChunkCache* pcache)
			: m_pcamera{ pcamera }
			, m_pchunk_cache{ pcache }
			, m_queue_mtx{}
			, m_load_queue{ ChunkDistance{ pcamera }, {} }
			, m_finished_queue{}
			, m_unload_queue{}
			, m_rendering_chunks{}
		{

		}
		virtual ~ChunkTransfer() = default;

		ChunkTransfer(ChunkTransfer const&) = delete;
		ChunkTransfer& operator= (ChunkTransfer const&) = delete;

		virtual void initialize() override
		{

		}

		virtual void update(Clock const& clock) override;

		void enqueue_to_load(Chunk* pchunk);
		bool dequeue_to_load(Chunk** ppchunk);

		void enqueue_to_render(Chunk* pchunk);
		bool dequeue_to_render(Chunk** ppchunk);

		void enqueue_to_unload(Chunk* pchunk);
		bool dequeue_to_unload(Chunk** ppchunk);
	};
}
