#include "fmega.h"
#include "fmegaObjectFactory.h"

#include "graphics/gpuBuffer.h"
#include "utils/random.h"

namespace fmega {

	Mesh* FMegaObjectFactory::GenBox(uint32 numInstances)
	{
		glm::vec4 color = glm::vec4(1, 1, 1, 1);
		std::vector<ColorVertex> vertices = {
			ColorVertex(glm::vec3(-1, -1, 0), color),
			ColorVertex(glm::vec3(1, -1, 0), color),
			ColorVertex(glm::vec3(1, 1, 0), color),
			ColorVertex(glm::vec3(-1, 1, 0), color),
		};

		std::vector<uint32> indices = {
			0, 1, 2,
			0, 2, 3
		};

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenHeart(uint32 numInstances)
	{
		glm::vec4 color = glm::vec4(0.9f, 0.2f, 0.3f, 1);
		std::vector<ColorVertex> vertices = {
			ColorVertex(glm::vec3(0, -1, 0), color),
			ColorVertex(glm::vec3(1, 0, 0), color),
			ColorVertex(glm::vec3(-1, 0, 0), color),
		};

		std::vector<uint32> indices = {
			0, 1, 2,
		};
		GenCircle(vertices, indices, color, glm::vec3(0.5f, 0, 0),  6, 0, 3, 0.5f);
		GenCircle(vertices, indices, color, glm::vec3(-0.5f, 0, 0), 6, 0, 3, 0.5f);
		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenShuriken(uint32 numInstances)
	{
		glm::vec4 innerColor = glm::vec4(0, 0, 0, 1);
		glm::vec4 outerColor = glm::vec4(1, 1, 0, 1);
		std::vector<ColorVertex> vertices = {
			ColorVertex(glm::vec3(0, 0, 0), innerColor),

			ColorVertex(glm::vec3(1, 0, 0), outerColor),
			ColorVertex(glm::vec3(1, 1, 0), outerColor),

			ColorVertex(glm::vec3(0, 1, 0), outerColor),
			ColorVertex(glm::vec3(-1, 1, 0), outerColor),

			ColorVertex(glm::vec3(-1, 0, 0), outerColor),
			ColorVertex(glm::vec3(-1, -1, 0), outerColor),

			ColorVertex(glm::vec3(0, -1, 0), outerColor),
			ColorVertex(glm::vec3(1, -1, 0), outerColor)
		};

		std::vector<uint32> indices = {
			0, 1, 2,
			0, 3, 4,
			0, 5, 6,
			0, 7, 8
		};

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenBallon(uint32 numInstances)
	{
		const int quality = 36;
		glm::vec4 color = glm::vec4(1, 0, 0, 1);

		const float lineWidth = 0.05f;
		const std::vector<glm::vec3> linePoints = {
			glm::vec3(-0.1f, -0.9f, 0),
			glm::vec3(0.2f, -1.3f, 0),
			glm::vec3(-0.2f, -1.6f, 0),
			glm::vec3(0.2f, -1.9f, 0),
		};

		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenCircle(vertices, indices, color, glm::vec3(0), quality, 0, quality, 1.f);

		for (size_t i = 1; i < linePoints.size(); i++) {
			const auto& p1 = linePoints[i - 1];
			const auto& p2 = linePoints[i];
			GenLine(vertices, indices, color, p1, p2, lineWidth);
		}
	
		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenArrow(uint32 numInstances) {

		glm::vec4 color = glm::vec4(1, 1, 1, 1);

		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		vertices.emplace_back(glm::vec3(1.1f,  0, 0), color);
		vertices.emplace_back(glm::vec3(0.7f,  0.2f, 0), color);
		vertices.emplace_back(glm::vec3(0.7f, -0.2f, 0), color);
		vertices.emplace_back(glm::vec3(0.9f, 0, 0), color);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(2);

		GenLine(vertices, indices, color, glm::vec3(-1, 0, 0), glm::vec3(0.9f, 0, 0), 0.04f);

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenBow(uint32 numInstances) {
		float width = 0.075;
		float offset = 0.075f;
		glm::vec4 color = glm::vec4(1, 0.7f, 0.7f, 1);

		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenLine(vertices, indices, color, glm::vec3(width + offset, 1 - width - offset, 0), glm::vec3(width + offset, -1 + width + offset, 0), width);
		GenCircleOutline(vertices, indices, color, glm::vec3(0), 36, -glm::half_pi<float>(), 18, 1.f - 2 * width, 1.f);

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenSegment(uint32 numInstances) {
		glm::vec4 color = glm::vec4(1, 1, 1, 1);

		std::vector<ColorVertex> vertices = {
			ColorVertex(glm::vec3(-1, 0, 0), color),
			ColorVertex(glm::vec3(-0.8f, 0.3f, 0), color),
			ColorVertex(glm::vec3(0.8f, 0.3f, 0), color),
			ColorVertex(glm::vec3(1, 0, 0), color),
			ColorVertex(glm::vec3(0.8f, -0.3f, 0), color),
			ColorVertex(glm::vec3(-0.8f, -0.3f, 0), color),
		};
		std::vector<uint32> indices = {
			0, 5, 1,
			1, 4, 2,
			4, 1, 5,
			4, 3, 2
		};
		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	PlatformData FMegaObjectFactory::GenPlatformData(
		int gridWidth, 
		int gridLength, 
		float width, 
		float length,
		float randomness) {

		PlatformData data;
		data.gridWidth = gridWidth;
		data.gridLength = gridLength;
		data.width = width;
		data.length = length;

		for (int i = 0; i <= gridLength; i++) {
			for (int j = 0; j <= gridWidth; j++) {
				float onEdge = i == 0 || i == gridLength || j == 0 || j == gridWidth;
				float z = (-float(i) / gridLength) * length;
				float x = (float(j) / gridWidth  - 0.5f) * width;
				data.topPositions.push_back(glm::vec2(x, z) + (1.f - onEdge) * (Random::NextVec2() * 2.f - 1.f) * randomness);
				data.bottomPositions.push_back(glm::vec2(x, z) + (1.f - onEdge) * (Random::NextVec2() * 2.f - 1.f) * randomness);
			}
		}

		return data;
	}

	void FMegaObjectFactory::GenPlatformMeshData(
		PlatformVertex* vertices,
		uint32* indices,
		int gridWidth, 
		int gridLength, 
		float width, 
		float length, 
		float randomness, 
		float thickness,
		const glm::vec3& color,
		float colorRandomness,
		float startingZ,
		glm::vec2 xyRandomness) {

		PlatformData data = GenPlatformData(gridWidth, gridLength, width, length, randomness);
		uint32 numVertices = 0;
		uint32 numIndices = 0;

		for (int i = 0; i < gridLength; i++) {
			for (int j = 0; j < gridWidth; j++) {
				int type = Random::NextUint(0, 2);
				glm::vec4 newColor1 = glm::vec4(color, 0) + glm::vec4(Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, 1);
				glm::vec4 newColor2 = glm::vec4(color, 0) + glm::vec4(Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, 1);
				GenFragment(vertices, numVertices, indices, numIndices, data, newColor1, j, i, thickness, startingZ, xyRandomness, (FragmentType)(type * 2 + 0));
				GenFragment(vertices, numVertices, indices, numIndices, data, newColor2, j, i, thickness, startingZ, xyRandomness, (FragmentType)(type * 2 + 1));
			}
		}
	}

	Mesh* FMegaObjectFactory::GenSphere(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenSphere(vertices, indices, glm::vec4(1, 1, 1, 1), glm::vec3(0), 1.f, 25);

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	// adds 18 vertices & 24 indices
	void FMegaObjectFactory::GenFragment(
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
		FragmentType type) {

		std::vector<int> inds;
		switch (type)
		{
		case fmega::FragmentType::TRIANGLE_11:
			inds = { 0, 1, 2 };
			break;
		case fmega::FragmentType::TRIANGLE_12:
			inds = { 1, 3, 2 };
			break;
		case fmega::FragmentType::TRIANGLE_21:
			inds = { 0, 1, 3 };
			break;
		case fmega::FragmentType::TRIANGLE_22:
			inds = { 0, 3, 2 };
			break;
		default:
			break;
		}

		glm::vec4 positions[6];
		glm::vec4 localOrigin = glm::vec4(0);
		glm::quat initialRotation = glm::quat(glm::vec3(
			Random::NextFloat() * glm::two_pi<float>(),
			Random::NextFloat() * glm::two_pi<float>(),
			Random::NextFloat() * glm::two_pi<float>()));
		glm::vec4 initialPosition = glm::vec4(
			Random::NextFloat() * xyRandomness.x - xyRandomness.x / 2.f,
			Random::NextFloat() * xyRandomness.y, startingZ, 1.f);

		for (int i = 0; i < 3; i++) {
			int indX = gridX + inds[i] % 2;
			int indZ = gridZ + inds[i] / 2;

			glm::vec2 pt = data.topPositions[indX + indZ * (data.gridWidth + 1)];
			glm::vec2 pb = data.bottomPositions[indX + indZ * (data.gridWidth + 1)];
			positions[i * 2 + 0] = glm::vec4(pt.x, 0, pt.y, 1);
			positions[i * 2 + 1] = glm::vec4(pb.x, -thickness, pb.y, 1);

			localOrigin += positions[i * 2 + 0];
			localOrigin += positions[i * 2 + 1];
		}

		localOrigin /= 6.f;

		// Sides
		for (int i = 0; i < 3; i++) {
			int indStart = numVertices;

			PlatformVertex p[4];
			p[0].position = positions[i * 2 + 0] - localOrigin;
			p[1].position = positions[i * 2 + 1] - localOrigin;
			p[2].position = positions[((i + 1) % 3) * 2 + 0] - localOrigin;
			p[3].position = positions[((i + 1) % 3) * 2 + 1] - localOrigin;

			glm::vec3 normal = GetNormal(p[0].position, p[1].position, p[2].position);
			for (int j = 0; j < 4; j++) {
				p[j].localOrigin = localOrigin;
				p[j].color = color;
				p[j].normal = glm::vec4(normal, 0);
				p[j].initialRotation = initialRotation;
				p[j].initialPosition = initialPosition;
				vertices[numVertices++] = p[j];
			}
			indices[numIndices++] = indStart + 0;
			indices[numIndices++] = indStart + 1;
			indices[numIndices++] = indStart + 2;
			indices[numIndices++] = indStart + 1;
			indices[numIndices++] = indStart + 3;
			indices[numIndices++] = indStart + 2;
		}

		// Top & Bottom
		for (int i = 0; i < 2; i++) {
			int indStart = numVertices;
			PlatformVertex p[3];
			p[0].position = positions[0 + i] - localOrigin;
			p[1].position = positions[2 + i] - localOrigin;
			p[2].position = positions[4 + i] - localOrigin;
			if (i == 1) {
				std::swap(p[0], p[1]);
			}

			glm::vec3 normal = GetNormal(p[0].position, p[1].position, p[2].position);
			for (int j = 0; j < 3; j++) {
				p[j].localOrigin = localOrigin;
				p[j].color = color;
				p[j].normal = glm::vec4(normal, 0);
				p[j].initialRotation = initialRotation;
				p[j].initialPosition = initialPosition;
				vertices[numVertices++] = p[j];
			}
			indices[numIndices++] = indStart + 0;
			indices[numIndices++] = indStart + 1;
			indices[numIndices++] = indStart + 2;
		}

	}

	glm::vec3 FMegaObjectFactory::GetNormal(
		const glm::vec3& p1,
		const glm::vec3& p2,
		const glm::vec3& p3) {
		
		return glm::normalize(glm::cross(p2 - p1, p3 - p1));
	}

	void FMegaObjectFactory::GenSphere(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& center,
		float radius,
		int quality) {

		int indStart = vertices.size();
		int ndiv = quality;
		float r = radius;

		for (int i = 1; i < ndiv - 1; i++) {
			for (int j = 0; j < ndiv; j++) {
				float a = float(j) / ndiv * glm::pi<float>() * 2.0f;
				float b = float(i) / ndiv * glm::pi<float>() - glm::half_pi<float>();
				glm::vec3 d = glm::vec3(
					cos(b) * sin(a),
					sin(b),
					cos(b) * cos(a));
				vertices.push_back(ColorVertex(center + r * d, color, d));
			}
		}

		int bottom = vertices.size();
		vertices.push_back(ColorVertex(center + glm::vec3(0, -r, 0), color, glm::vec3(0, -1, 0)));
		int top = vertices.size();
		vertices.push_back(ColorVertex(center + glm::vec3(0, r, 0), color, glm::vec3(0, 1, 0)));

		// Bottom triangles
		for (int i = 0; i < ndiv; i++) {
			indices.push_back(indStart + i);
			indices.push_back(indStart + bottom);
			indices.push_back(indStart + (i + 1) % ndiv);
		}

		// Top triangles
		for (int i = 0; i < ndiv; i++) {
			indices.push_back(indStart + top);
			indices.push_back(indStart + (ndiv - 3) * ndiv + i);
			indices.push_back(indStart + (ndiv - 3) * ndiv + (i + 1) % ndiv);
		}

		for (int i = 1; i < ndiv - 2; i++) {
			for (int j = 0; j < ndiv; j++) {
				indices.push_back(indStart + (i - 1) * ndiv + j);
				indices.push_back(indStart + (i - 1) * ndiv + (j + 1) % ndiv);
				indices.push_back(indStart + i * ndiv + j);

				indices.push_back(indStart + (i - 1) * ndiv + (j + 1) % ndiv);
				indices.push_back(indStart + i * ndiv + (j + 1) % ndiv);
				indices.push_back(indStart + i * ndiv + j);
			}
		}
	}

	void FMegaObjectFactory::GenLine(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& p1,
		const glm::vec3& p2,
		float lineWidth) {

		glm::vec3 dir = glm::normalize(p2 - p1);
		glm::vec3 right = glm::vec3(dir.y, -dir.x, 0);

		int indStart = vertices.size();

		vertices.emplace_back(p1 - right * lineWidth - dir * lineWidth, color);
		vertices.emplace_back(p1 + right * lineWidth - dir * lineWidth, color);
		vertices.emplace_back(p2 + right * lineWidth + dir * lineWidth, color);
		vertices.emplace_back(p2 - right * lineWidth + dir * lineWidth, color);

		indices.push_back(indStart);
		indices.push_back(indStart + 1);
		indices.push_back(indStart + 2);
		indices.push_back(indStart);
		indices.push_back(indStart + 2);
		indices.push_back(indStart + 3);
	}

	void FMegaObjectFactory::GenCircle(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& offset,
		int quality,
		float angleOffset,
		int numTriangles,
		float radius) {

		int indStart = vertices.size();
		vertices.emplace_back(glm::vec3(0, 0, 0), color);

		for (int i = 0; i < glm::min(numTriangles + 1, quality); i++) {
			float theta = angleOffset + 2.f * float(i) / quality * glm::pi<float>();
			vertices.emplace_back(offset + glm::vec3(cosf(theta), sinf(theta), 0) * radius, color);
		}

		for (int i = 0; i < numTriangles; i++) {
			indices.push_back(indStart);
			indices.push_back(indStart + i + 1);
			indices.push_back(indStart + (i + 1) % quality + 1);
		}
	}

	void FMegaObjectFactory::GenCircleOutline(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& offset,
		int quality,
		float angleOffset,
		int numTriangles,
		float innerRadius,
		float outerRadius) {

		int indStart = vertices.size();

		for (int i = 0; i < glm::min(numTriangles + 1, quality); i++) {
			float theta = angleOffset + 2.f * float(i) / quality * glm::pi<float>();

			vertices.emplace_back(offset + innerRadius * glm::vec3(cosf(theta), sinf(theta), 0), color);
			vertices.emplace_back(offset + outerRadius * glm::vec3(cosf(theta), sinf(theta), 0), color);
		}

		for (int i = 0; i < numTriangles; i++) {
			indices.push_back(indStart + i * 2 + 0);
			indices.push_back(indStart + i * 2 + 1);
			indices.push_back(indStart + ((i + 1) % quality) * 2 + 1);

			indices.push_back(indStart + i * 2 + 0);
			indices.push_back(indStart + ((i + 1) % quality) * 2 + 1);
			indices.push_back(indStart + ((i + 1) % quality) * 2 + 0);
		}
	}

	Mesh* FMegaObjectFactory::GenObject(const std::vector<ColorVertex>& vertices, const std::vector<uint32>& indices, PrimitiveType primitiveType, uint32 numInstances)
	{
		GPUBuffer* ibo = new GPUBuffer(BufferType::ELEMENT_ARRAY, (byte*)indices.data(), indices.size() * sizeof(uint32), BufferUsage::STATIC_DRAW);
		GPUBuffer* vbo = new GPUBuffer(BufferType::ARRAY, (byte*)vertices.data(), vertices.size() * sizeof(ColorVertex), BufferUsage::STATIC_DRAW);
		GPUBuffer* instbo = new GPUBuffer(BufferType::ARRAY, nullptr, numInstances * sizeof(MeshRenderData), BufferUsage::DYNAMIC_DRAW);
		Mesh* m = new Mesh(primitiveType);
		m->SetIBO(ibo, indices.size());

		m->AddBuffer(vbo);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->FinishBuffer();

		m->AddBuffer(instbo);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4, false, 1);
		m->FinishBuffer();

		return m;
	}

	Mesh* FMegaObjectFactory::GenPlatformObject(
		const std::vector<PlatformVertex>& vertices,
		const std::vector<uint32>& indices,
		PrimitiveType primitiveType) {

		GPUBuffer* ibo = new GPUBuffer(BufferType::ELEMENT_ARRAY, (byte*)indices.data(), indices.size() * sizeof(uint32), BufferUsage::STATIC_DRAW);
		GPUBuffer* vbo = new GPUBuffer(BufferType::ARRAY, (byte*)vertices.data(), vertices.size() * sizeof(PlatformVertex), BufferUsage::STATIC_DRAW);
		
		return GenPlatformObject(vbo, ibo, indices.size(), primitiveType);
	}

	Mesh* FMegaObjectFactory::GenPlatformObject(
		GPUBuffer* vbo,
		GPUBuffer* ibo,
		uint32 numIndices,
		PrimitiveType primitiveType) {

		Mesh* m = new Mesh(primitiveType);
		m->SetIBO(ibo, numIndices);

		m->AddBuffer(vbo);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->FinishBuffer();

		return m;
	}

}