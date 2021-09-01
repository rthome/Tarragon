#pragma once

#include "component.h"
#include "chunk.h"

namespace tarragon
{
	struct ChunkVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	struct ChunkMesh
	{
		glm::vec3 WorldPosition;
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec4> Colors;
		std::vector<glm::vec3> Normals;
		std::vector<uint32_t> Indices;
	};

	class ChunkTransfer : public UpdateComponent
	{
	public:
		virtual void update(Clock const& clock) override;
	};
}
