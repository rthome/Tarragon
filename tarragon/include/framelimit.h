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
		Clock::time_point _start;
		Clock::duration _frame_duration;

	public:
		explicit FrameLimit(uint32_t max_fps)
			: _start{ Clock::now() }
			, _frame_duration{ Clock::duration{ Clock::duration::period::den / max_fps } }
		{
		}

		~FrameLimit()
		{
			std::this_thread::sleep_until(_start + _frame_duration);
		}
	};
}
