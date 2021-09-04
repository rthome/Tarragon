#pragma once

#include "component.h"
#include "chunk.h"

namespace tarragon
{
	struct ChunkMesh
	{
		glm::vec3 WorldPosition;
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<uint32_t> Indices;
	};

	class ChunkTransfer : public UpdateComponent
	{
	public:
		virtual void update(Clock const& clock) override;
	};
}
