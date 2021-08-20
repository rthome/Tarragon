#pragma once

#include <chrono>
#include <thread>

namespace tarragon
{
	class FrameLimit final
	{
	public:
		using Clock = std::chrono::steady_clock;
		
	private:
		Clock::time_point m_start;
		Clock::duration m_frame_duration;

	public:
		explicit FrameLimit(uint32_t max_fps)
			: m_start{ Clock::now() }
			, m_frame_duration{ Clock::duration{ Clock::duration::period::den / max_fps } }
		{
		}

		~FrameLimit()
		{
			std::this_thread::sleep_until(m_start + m_frame_duration);
		}
	};
}
