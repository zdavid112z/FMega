#pragma once

#include "fmega.h"

namespace fmega {

	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texCoord;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct MeshData
	{
		Vertex* vertices = nullptr;
		uint32* indices = nullptr;
		uint32 numVertices, numIndices;
	};

	enum class MeshVariableType
	{
		INT8,
		UINT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		FLOAT32
	};

	enum class PrimitiveType
	{
		TRIANGLES,
		TRIANGLE_STRIP,
		LINES
	};

}