#pragma once

#include <noise/modules.h>
#include "chunk.h"

namespace tarragon
{
	class World
	{
	private:
		Module m_source;
		double m_air_threshold{ 0.1 };

		Block map_value(double value);

	public:
		World();

		void generate_data(Chunk* pchunk);
	};
}
