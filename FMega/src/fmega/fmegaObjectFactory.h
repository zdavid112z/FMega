#pragma once

#include "graphics/mesh.h"
#include "graphics/gpuBuffer.h"
#include "utils/colors.h"

namespace fmega {

	#pragma pack(push,1)
	struct MeshRenderData {
		glm::mat4 model = glm::mat4(1);
		glm::vec4 color = glm::vec4(0);
		glm::vec4 opacity = glm::vec4(1);
	};

	struct ColorVertex {
		ColorVertex() {}
		ColorVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec3& normal = glm::vec3(0, 1, 0))
			: position(glm::vec4(position, 1.f)), color(color), normal(glm::vec4(normal, 0)) {}
		glm::vec4 position = glm::vec4(0);
		glm::vec4 color = glm::vec4(0);
		glm::vec4 normal = glm::vec4(0, 1, 0, 0);
	};

	struct PlatformVertex {
		glm::vec4 position = glm::vec4(0);
		glm::vec4 color = glm::vec4(0);
		glm::vec4 normal = glm::vec4(0, 1, 0, 0);
		glm::quat initialRotation = glm::quat();
		glm::vec4 initialPosition = glm::vec4(0);
		glm::vec4 localOrigin = glm::vec4(0);
	};
	#pragma pack(pop)

	struct PlatformData {
		float width = 0.f, length = 0.f;
		int gridWidth = 0, gridLength = 0;
		std::vector<glm::vec2> topPositions;
		std::vector<glm::vec2> bottomPositions;
	};

	struct PlatformGenData {

		int gridWidth;
		int gridLength;
		float width;
		float length;
		float randomness;
		float thickness;
		glm::vec3 color;
		float colorRandomness;
		float startingZ;
		glm::vec2 xyRandomness;

		PlatformVertex* vertices;
		uint32* indices;
	};

	enum class FragmentType {
		TRIANGLE_11 = 0,
		TRIANGLE_12 = 1,
		TRIANGLE_21 = 2,
		TRIANGLE_22 = 3,
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
		static void GenPlatformMeshData(
			PlatformVertex* vertices,
			uint32* indices,
			int gridWidth, int gridLength, 
			float width, float length, 
			float randomness, float thickness, 
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
			float width, 
			float length, 
			float randomness);
		static void GenFragment(
			PlatformVertex* vertices,
			uint32& numVertices,
			uint32* indices,
			uint32& numIndices,
			const PlatformData& data,
			const glm::vec4& color,
			int gridX, int gridZ,
			float thickness,
			float startingZ,
			glm::vec2 xyRandomness,
			FragmentType type);
		static void GenSphere(
			std::vector<ColorVertex>& vertices,
			std::vector<uint32>& indices,
			const glm::vec4& color,
			const glm::vec3& center,
			float radius,
			int quality);
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