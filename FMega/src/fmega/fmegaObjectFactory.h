#pragma once

#include "graphics/mesh.h"
#include "graphics/gpuBuffer.h"
#include "utils/colors.h"

namespace fmega {

	#pragma pack(push,1)
	struct MeshRenderData {
		glm::mat4 model = glm::mat4(1);
		glm::vec4 color = glm::vec4(0);
		union {
			struct {
				float opacity;
				float textureOpacity;
				float unused1;
				float unused2;
			};
			glm::vec4 opacityVec = glm::vec4(1, 0, 0, 0);
		};
	};

	struct ColorVertex {
		ColorVertex() {}
		ColorVertex(
			const glm::vec3& position, 
			const glm::vec4& color, 
			const glm::vec3& normal = glm::vec3(0, 1, 0)) : 

			position(glm::vec4(position, 1.f)), 
			color(color), 
			normal(glm::vec4(normal, 0)) {}

		glm::vec4 position = glm::vec4(0);
		glm::vec4 uv = glm::vec4(0);
		glm::vec4 color = glm::vec4(0);
		glm::vec4 normal = glm::vec4(0, 1, 0, 0);
		glm::vec4 tangent = glm::vec4(0);
	};

	struct PlatformVertex {
		glm::vec4 position = glm::vec4(0);
		glm::vec4 uv = glm::vec4(0);
		glm::vec4 color = glm::vec4(0);
		glm::vec4 normal = glm::vec4(0, 1, 0, 0);
		glm::vec4 tangent = glm::vec4(0);
		glm::quat initialRotation = glm::quat();
		glm::vec4 initialPosition = glm::vec4(0);
		glm::vec4 localOrigin = glm::vec4(0);
	};
	#pragma pack(pop)

	struct PlatformData {
		float width = 0.f, length = 0.f, height = 0.f;
		int gridWidth = 0, gridLength = 0, gridHeight = 0;
		std::vector<glm::vec3> positions;
	};

	struct PlatformGenData {

		int gridWidth;
		int gridLength;
		int gridHeight;
		float width;
		float length;
		float height;
		float randomness;
		glm::vec3 color;
		float colorRandomness;
		float startingZ;
		glm::vec2 xyRandomness;

		PlatformVertex* vertices;
		uint32* indices;
	};

	class FMegaObjectFactory {
	public:
		static Mesh* GenShuriken(uint32 numInstances);
		static Mesh* GenBallon(uint32 numInstances);
		static Mesh* GenArrow(uint32 numInstances);
		static Mesh* GenBow(uint32 numInstances);
		static Mesh* GenBox(uint32 numInstances);
		static Mesh* GenSegment(uint32 numInstances);
		static Mesh* GenHeart(uint32 numInstances);
		static Mesh* GenSphere(uint32 numInstances);
		static Mesh* GenPickup(uint32 numInstances);
		static void GenPlatformMeshData(
			PlatformVertex* vertices,
			uint32* indices,
			int gridWidth, int gridLength, int gridHeight,
			float width, float length, float height,
			float randomness,
			const glm::vec3& color,
			float colorRandomness,
			float startingZ,
			glm::vec2 xyRandomness);

		static glm::vec3 GetNormal(
			const glm::vec3& p1,
			const glm::vec3& p2,
			const glm::vec3& p3);
		static PlatformData GenPlatformData(
			int gridWidth,
			int gridLength,
			int gridHeight,
			float width,
			float length,
			float height,
			float randomness);
		static void GenFragment(
			PlatformVertex* vertices,
			uint32& numVertices,
			uint32* indices,
			uint32& numIndices,
			const PlatformData& data,
			const glm::vec4& color,
			int gridX, int gridY, int gridZ,
			float startingZ,
			glm::vec2 xyRandomness,
			int typeId, bool flip);
		static void GenSphere(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& center,
			float radius,
			int quality);
		static void GenSmoothCube(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& center,
			float size,
			int quality,
			float smoothAmount);
		static void GenSmoothCubeFacesAndNormals(
			int quality, 
			std::map<std::tuple<int, int, int>, int>& posToId, 
			std::vector<ColorVertex>& vertices, 
			std::vector<uint32>& indices);
		static void GenSmoothCubeVertexPositions(
			int quality, 
			std::map<std::tuple<int, int, int>, int>& posToId, 
			float smoothAmount, 
			const glm::vec4& color, 
			std::vector<ColorVertex>& vertices,
			float size);
		static void GenPosToId(
			int indStart,
			int quality, 
			std::map<std::tuple<int, int, int>, int>& posToId, 
			int& nextId);
		static void GenFace(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			int v0, int v1, int v2,
			bool isOutward);
		static void GenTangents(
			ColorVertex* vertices,
			uint32 numVertices,
			uint32* indices,
			uint32 numIndices);
		static void GenTangents(
			PlatformVertex* vertices,
			uint32 numVertices,
			uint32* indices,
			uint32 numIndices);
		static void GenLine(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& p1,
			const glm::vec3& p2,
			float lineWidth);
		static void GenCircle(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& offset,
			int quality,
			float angleOffset,
			int numTriangles,
			float radius);
		static void GenCircleOutline(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& offset,
			int quality,
			float angleOffset,
			int numTriangles,
			float innerRadius,
			float outerRadius);
		static Mesh* GenObject(
			const std::vector<ColorVertex>& vertices, 
			const std::vector<uint32>& indices, 
			PrimitiveType primitiveType = PrimitiveType::TRIANGLES,
			uint32 numInstances = 1);
		static Mesh* GenPlatformObject(
			const std::vector<PlatformVertex>& vertices,
			const std::vector<uint32>& indices,
			PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
		static Mesh* GenPlatformObject(
			GPUBuffer* vbo,
			GPUBuffer* ibo,
			uint32 numIndices,
			PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
	};

}
