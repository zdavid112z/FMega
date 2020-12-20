#pragma once

#include "meshShared.h"

namespace fmega {

	class MeshFactory
	{
	public:
		// 'vertices' and 'indices' will contain 36 elements each
		static void CreateCube(Vertex* vertices, uint32* indices, float length);
		static void CalculateTangentsAndBitangents(Vertex* vertices, uint32 numVertices, uint32* indices, uint32 numIndices);
	};

}