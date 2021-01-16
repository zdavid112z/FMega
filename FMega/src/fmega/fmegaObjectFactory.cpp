#include "fmega.h"
#include "fmegaObjectFactory.h"

#include "graphics/gpuBuffer.h"
#include "utils/random.h"

#include "entities/tunnel.h"

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
		vertices[0].uv = glm::vec4(0, 1, 0, 0);
		vertices[1].uv = glm::vec4(1, 1, 0, 0);
		vertices[2].uv = glm::vec4(1, 0, 0, 0);
		vertices[3].uv = glm::vec4(0, 0, 0, 0);

		std::vector<uint32> indices = {
			0, 1, 2,
			0, 2, 3
		};

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenPickup(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenSmoothCube(vertices, indices, glm::vec4(1, 1, 1, 1), glm::vec3(0), 1.f, 8, 1.5f);

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}
	
	Mesh* FMegaObjectFactory::GenTunnel(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		float r = Tunnel::Radius;
		int xyQuality = 30;
		float minLength = 1000;
		float uvScale = 5.f;
		glm::vec4 color(0.f, 0.f, 0.f, 1.f);
		glm::vec3 center(0);

		GenCylinder(vertices, indices, color, center, r, xyQuality, minLength, uvScale, 2, true);
		GenTangents(vertices.data(), vertices.size(), indices.data(), indices.size());

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	Mesh* FMegaObjectFactory::GenBars(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		float radius = 0.15f;
		int xyQuality = 8;
		float uvScale = 1.f;
		glm::vec4 color(0.f, 0.f, 0.f, 1.f);
		float minVertLength = Tunnel::Radius;
		float minHorLength = Tunnel::Radius * 2.01;

		int numVertBars = 25;
		int numHorBars = 13;

		float angleStep = glm::two_pi<float>() / xyQuality;
		float quadSize = radius * sqrt(2 - 2 * glm::cos(angleStep));
		int zQualityHor = int(glm::ceil(minHorLength / quadSize)) + 1;
		int zQualityVert = int(glm::ceil(minVertLength / quadSize)) + 1;
		float lengthHor = (zQualityHor - 1) * quadSize;
		float lengthVert = (zQualityVert - 1) * quadSize;

		for (int i = 0; i < numVertBars; i++) {
			float amount = float(i) / float(numVertBars - 1);
			GenCylinder(vertices, indices, color, 
				glm::vec3(amount * lengthHor - lengthHor / 2.f , 0, 0), 
				radius, xyQuality, minVertLength, uvScale, 1, false);
		}

		for (int i = 0; i < numHorBars; i++) {
			float amount = float(i) / float(numHorBars - 1);
			GenCylinder(vertices, indices, color, 
				glm::vec3(0, amount * lengthVert - lengthVert / 2.f, 0),
				radius, xyQuality, minHorLength, uvScale, 0, false);
		}

		GenTangents(vertices.data(), vertices.size(), indices.data(), indices.size());

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	void FMegaObjectFactory::GenCylinder(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& center,
		float radius,
		int xyQuality,
		float minLength,
		float uvScale,
		int axis,
		bool normalsInwards) {

		int indStart = vertices.size();

		float angleStep = glm::two_pi<float>() / xyQuality;
		float quadSize = radius * sqrt(2 - 2 * glm::cos(angleStep));
		int zQuality = int(glm::ceil(minLength / quadSize)) + 1;
		float length = (zQuality - 1) * quadSize;
		float length2 = length * 0.5f;
		float normalModifier = normalsInwards ? -1 : 1;

		for (int i = 0; i < zQuality; i++) {
			float amountZ = float(i) / xyQuality;
			float z = float(i) * quadSize - length2;
			for (int j = 0; j <= xyQuality; j++) {
				float amountXY = float(j) / xyQuality;
				float theta = amountXY * glm::two_pi<float>();
				float c = glm::cos(theta);
				float s = glm::sin(theta);
				glm::vec3 p = glm::vec3(radius * c, radius * s, z) + center;
				glm::vec3 n = glm::vec3(c, s, 0) * normalModifier;
				if (axis == 0) {
					std::swap(p.x, p.z);
					std::swap(n.x, n.z);
				}
				else if (axis == 1) {
					std::swap(p.y, p.z);
					std::swap(n.y, n.z);
				}

				ColorVertex v;
				v.position = glm::vec4(p, 1.f);
				v.color = color;
				v.uv = glm::vec4(amountXY * uvScale, amountZ * uvScale, 0.f, 0.f);
				v.normal = glm::vec4(n, 0.f);
				v.tangent = glm::vec4(0);
				vertices.push_back(v);
			}
		}

		for (int i = 0; i < zQuality - 1; i++) {
			for (int j = 0; j < xyQuality; j++) {
				int inds[] = {
					indStart + (j + 0) + (i + 0) * (xyQuality + 1),
					indStart + (j + 0) + (i + 1) * (xyQuality + 1),
					indStart + (j + 1) + (i + 0) * (xyQuality + 1),
					indStart + (j + 1) + (i + 1) * (xyQuality + 1),
				};
				GenFace(vertices, indices, inds[0], inds[1], inds[2], !normalsInwards, false, center);
				GenFace(vertices, indices, inds[1], inds[2], inds[3], !normalsInwards, false, center);
			}
		}

	}

	void FMegaObjectFactory::GenSmoothCube(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		const glm::vec4& color,
		const glm::vec3& center,
		float size,
		int quality,
		float smoothAmount) {

		int indStart = vertices.size();

		int nextId = 0;
		std::map<std::tuple<int, int, int>, int> posToId;

		GenPosToId(indStart, quality, posToId, nextId);
		vertices.resize(vertices.size() + nextId);

		GenSmoothCubeVertexPositions(quality, posToId, smoothAmount, color, vertices, size);
		GenSmoothCubeFacesAndNormals(quality, posToId, vertices, indices);
		GenTangents(vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	void FMegaObjectFactory::GenSmoothCubeFacesAndNormals(int quality, std::map<std::tuple<int, int, int>, int>& posToId, std::vector<fmega::ColorVertex>& vertices, std::vector<uint32>& indices)
	{
		for (int f = 0; f < 6; f++) {
			for (int i = 0; i < quality - 1; i++) {
				for (int j = 0; j < quality - 1; j++) {
					glm::ivec3 c;
					glm::ivec3 d1, d2;
					switch (f)
					{
					case 0: c = glm::ivec3(i, j, 0);           d1 = glm::ivec3(1, 0, 0); d2 = glm::ivec3(0, 1, 0); break;
					case 1: c = glm::ivec3(i, j, quality - 1); d1 = glm::ivec3(1, 0, 0); d2 = glm::ivec3(0, 1, 0); break;
					case 2: c = glm::ivec3(i, 0, j);           d1 = glm::ivec3(1, 0, 0); d2 = glm::ivec3(0, 0, 1); break;
					case 3: c = glm::ivec3(i, quality - 1, j); d1 = glm::ivec3(1, 0, 0); d2 = glm::ivec3(0, 0, 1); break;
					case 4: c = glm::ivec3(0, i, j);           d1 = glm::ivec3(0, 1, 0); d2 = glm::ivec3(0, 0, 1); break;
					case 5: c = glm::ivec3(quality - 1, i, j); d1 = glm::ivec3(0, 1, 0); d2 = glm::ivec3(0, 0, 1); break;
					}
					glm::ivec3 coords[4] = {
						c, c + d1, c + d2, c + d1 + d2
					};
					int ids[4];
					for (int k = 0; k < 4; k++) {
						ids[k] = posToId[std::make_tuple(coords[k].x, coords[k].y, coords[k].z)];
					}
					GenFace(vertices, indices, ids[0], ids[1], ids[2], true);
					GenFace(vertices, indices, ids[1], ids[2], ids[3], true);
				}
			}
		}

		for (const auto& it : posToId) {
			vertices[it.second].normal = glm::normalize(vertices[it.second].normal);
		}
	}

	Mesh* FMegaObjectFactory::GenLamp(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenSmoothCube(vertices, indices, glm::vec4(1, 1, 1, 1), glm::vec3(0), 1.f, 3, 0.f);

		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	void FMegaObjectFactory::GenFace(
		std::vector<ColorVertex>& vertices,
		std::vector<uint32>& indices,
		int v0, int v1, int v2,
		bool isOutward, bool addNormal,
		const glm::vec3& objCenter) {

		glm::vec3 p[3] = { vertices[v0].position, vertices[v1].position, vertices[v2].position };
		glm::vec3 normal = GetNormal(p[0], p[1], p[2]);
		glm::vec3 oToFaceCenter = (p[0] + p[1] + p[2]) / 3.f - objCenter;
		float d = glm::dot(normal, oToFaceCenter);
		if ((d > 0) ^ isOutward) {
			normal = -normal;
			std::swap(v0, v1);
		}

		if (addNormal) {
			vertices[v0].normal += glm::vec4(normal, 0.f);
			vertices[v1].normal += glm::vec4(normal, 0.f);
			vertices[v2].normal += glm::vec4(normal, 0.f);
		}

		indices.push_back(v0);
		indices.push_back(v1);
		indices.push_back(v2);
	}

	void FMegaObjectFactory::GenSmoothCubeVertexPositions(
		int quality, 
		std::map<std::tuple<int, int, int>, int>& posToId, 
		float smoothAmount, 
		const glm::vec4& color, 
		std::vector<fmega::ColorVertex>& vertices,
		float size)
	{
		for (int i = 0; i < quality; i++) {
			for (int j = 0; j < quality; j++) {
				for (int k = 0; k < quality; k++) {
					bool edgeI = i == 0 || i == quality - 1;
					bool edgeJ = j == 0 || j == quality - 1;
					bool edgeK = k == 0 || k == quality - 1;
					if (!edgeI && !edgeJ && !edgeK) continue;

					int myId = posToId[std::make_tuple(i, j, k)];

					glm::vec3 sum = glm::vec3(0);
					int count = 0;
					for (int e = 0; e < 27; e++) {
						if (e == 1 + 3 + 9) continue;
						int ni = i + ((e / 1) % 3) - 1;
						int nj = j + ((e / 3) % 3) - 1;
						int nk = k + ((e / 9) % 3) - 1;
						auto it = posToId.find(std::make_tuple(ni, nj, nk));
						if (it == posToId.end()) continue;
						count++;
						sum += glm::vec3(ni, nj, nk) / float(quality - 1);
					}
					/*for (int e = 0; e < 125; e++) {
						if (e == 1 + 5 + 25) continue;
						int ni = i + ((e / 1) % 5)  - 2;
						int nj = j + ((e / 5) % 5)  - 2;
						int nk = k + ((e / 25) % 5) - 2;
						auto it = posToId.find(std::make_tuple(ni, nj, nk));
						if (it == posToId.end()) continue;
						count++;
						sum += glm::vec3(ni, nj, nk) / float(quality - 1);
					}*/
					sum /= count;
					glm::vec3 op = glm::vec3(i, j, k) / float(quality - 1);
					glm::vec3 p = glm::lerp(op, sum, smoothAmount);

					ColorVertex v;
					v.position = glm::vec4((p - glm::vec3(0.5f)) * size, 1.f);
					v.color = color;
					v.normal = glm::vec4(0.f);

					vertices[myId] = v;
				}
			}
		}
	}

	void FMegaObjectFactory::GenPosToId(int indStart, int quality, std::map<std::tuple<int, int, int>, int>& posToId, int& nextId)
	{
		for (int i = 0; i < quality; i++) {
			for (int j = 0; j < quality; j++) {
				for (int k = 0; k < quality; k++) {
					bool edgeI = i == 0 || i == quality - 1;
					bool edgeJ = j == 0 || j == quality - 1;
					bool edgeK = k == 0 || k == quality - 1;
					if (!edgeI && !edgeJ && !edgeK) continue;
					posToId[std::make_tuple(i, j, k)] = nextId + indStart;
					nextId++;
				}
			}
		}
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
		int gridHeight,
		float width, 
		float length,
		float height,
		float randomness) {

		PlatformData data;
		data.gridWidth = gridWidth;
		data.gridLength = gridLength;
		data.gridHeight = gridHeight;
		data.width = width;
		data.length = length;
		data.height = height;

		for (int h = 0; h <= gridHeight; h++) {
			for (int i = 0; i <= gridLength; i++) {
				for (int j = 0; j <= gridWidth; j++) {
					float onEdgeX = j == 0 || j == gridWidth;
					float onEdgeY = h == 0 || h == gridHeight;
					float onEdgeZ = i == 0 || i == gridLength;
					glm::vec3 onEdge = glm::vec3(onEdgeX, onEdgeY, onEdgeZ);
					glm::vec3 noise = (1.f - onEdge) * (Random::NextVec3() * 2.f - 1.f) * randomness;
					float z = (-float(i) / gridLength) * length;
					float x = (float(j) / gridWidth - 0.5f) * width;
					float y = (-float(h) / gridHeight) * height;
					data.positions.push_back(glm::vec3(x, y, z) + noise);
				}
			}
		}

		return data;
	}

	void FMegaObjectFactory::GenPlatformMeshData(
		PlatformVertex* vertices,
		uint32* indices,
		int gridWidth, 
		int gridLength,
		int gridHeight,
		float width, 
		float length,
		float height,
		float randomness, 
		const glm::vec3& color,
		float colorRandomness,
		float startingZ,
		glm::vec2 xyRandomness) {

		PlatformData data = GenPlatformData(gridWidth, gridLength, gridHeight, width, length, height, randomness);
		uint32 numVertices = 0;
		uint32 numIndices = 0;

		for (int h = 0; h < gridHeight; h++) {
			for (int i = 0; i < gridLength; i++) {
				for (int j = 0; j < gridWidth; j++) {
					bool flip = Random::NextUint(0, 2) == 0;
					flip = false;
					for (int k = 0; k < 6; k++) {
						glm::vec4 newColor = glm::vec4(color, 0) + glm::vec4(Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, Random::NextFloat() * colorRandomness, 1);
						GenFragment(vertices, numVertices, indices, numIndices, data, newColor, j, h, i, startingZ, xyRandomness, k, flip);
					}
				}
			}
		}

		GenTangents(vertices, numVertices, indices, numIndices);
	}

	Mesh* FMegaObjectFactory::GenSphere(uint32 numInstances) {
		std::vector<ColorVertex> vertices;
		std::vector<uint32> indices;

		GenSphere(vertices, indices, glm::vec4(1, 1, 1, 1), glm::vec3(0), 1.f, 25);
		GenTangents(vertices.data(), vertices.size(), indices.data(), indices.size());
		return GenObject(vertices, indices, PrimitiveType::TRIANGLES, numInstances);
	}

	// adds 12 vertices & 12 indices
	void FMegaObjectFactory::GenFragment(
		PlatformVertex* vertices,
		uint32& numVertices,
		uint32* indices,
		uint32& numIndices,
		const PlatformData& data,
		const glm::vec4& color,
		int gridX, int gridY, int gridZ,
		float startingZ,
		glm::vec2 xyRandomness,
		int typeId, bool flip) {

		int allInds[6][4] = {
			{ 0, 1, 3, 5 },
			{ 0, 3, 4, 5 },
			{ 3, 4, 5, 7 },
			{ 0, 2, 3, 6 },
			{ 0, 3, 4, 6 },
			{ 3, 4, 6, 7 }
		};
		std::vector<int> inds(allInds[typeId], allInds[typeId] + 4);
		if (flip) {
			for (int& id : inds) {
				id ^= 1;
			}
		}

		glm::vec4 positions[4];
		glm::vec3 localOrigin = glm::vec3(0);
		glm::quat initialRotation = glm::quat(glm::vec3(
			Random::NextFloat() * glm::two_pi<float>(),
			Random::NextFloat() * glm::two_pi<float>(),
			Random::NextFloat() * glm::two_pi<float>()));
		glm::vec4 initialPosition = glm::vec4(
			Random::NextFloat() * xyRandomness.x - xyRandomness.x / 2.f,
			Random::NextFloat() * xyRandomness.y, startingZ, 1.f);

		int sz = data.gridWidth + 1;
		int sy = (data.gridWidth + 1) * (data.gridLength + 1);
		for (int i = 0; i < 4; i++) {
			int dx = (inds[i] & 1) != 0;
			int dy = (inds[i] & 4) != 0;
			int dz = (inds[i] & 2) != 0;
			int indX = gridX + dx;
			int indY = gridY + dy;
			int indZ = gridZ + dz;
			int ind = indX + indZ * sz + indY * sy;

			glm::vec4 pc = glm::vec4(data.positions[ind], 1.0f);
			positions[i] = pc;

			localOrigin += glm::vec3(pc);
		}

		localOrigin /= 4.f;

		for (int i = 0; i < 4; i++) {
			int indStart = numVertices;
			PlatformVertex p[3];
			glm::vec3 faceCenter = glm::vec3(0.f);
			for (int j = 0; j < 3; j++) {
				p[j].position = positions[(i + j) % 4];
				faceCenter += glm::vec3(p[j].position);
			}
			faceCenter /= 3.f;
			glm::vec3 objToFace = faceCenter - localOrigin;
			glm::vec3 normal = GetNormal(p[0].position, p[1].position, p[2].position);
			float d = glm::dot(normal, objToFace);
			if (d < 0) {
				normal = -normal;
				std::swap(p[0].position, p[1].position);
			}

			for (int j = 0; j < 3; j++) {
				p[j].uv = glm::vec4(p[j].position.x, p[j].position.z + p[j].position.y, 0.f, 0.f) * 0.15f;
				p[j].position -= glm::vec4(localOrigin, 0.f);
				p[j].localOrigin = glm::vec4(localOrigin, 1.f);
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

		/*
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
		}*/

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
				float a = float(j) / ndiv * glm::two_pi<float>();
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

	void FMegaObjectFactory::GenTangents(
		ColorVertex* vertices,
		uint32 numVertices,
		uint32* indices,
		uint32 numIndices) {

		for (uint32 i = 0; i < numIndices; i += 3) {
			glm::vec4 v0 = vertices[indices[i + 0]].position;
			glm::vec4 v1 = vertices[indices[i + 1]].position;
			glm::vec4 v2 = vertices[indices[i + 2]].position;

			// Shortcuts for UVs
			glm::vec4 uv0 = vertices[indices[i + 0]].uv;
			glm::vec4 uv1 = vertices[indices[i + 1]].uv;
			glm::vec4 uv2 = vertices[indices[i + 2]].uv;

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec4 tangent = glm::vec4((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r, 0.f);
			tangent = glm::normalize(tangent);

			vertices[indices[i + 0]].tangent += tangent;
			vertices[indices[i + 1]].tangent += tangent;
			vertices[indices[i + 2]].tangent += tangent;
		}

		for (uint32 i = 0; i < numVertices; i++) {
			auto& v = vertices[i];
			v.tangent = glm::normalize(v.tangent);

			if (glm::isnan(v.tangent) != glm::bvec4(0) || glm::abs(glm::length2(v.tangent) - 1.f) >= 1e-4f) {
				v.tangent = v.normal + glm::vec4(1, 1, 1, 0);
			}

			v.tangent = glm::normalize(v.tangent - glm::dot(v.tangent, v.normal) * v.normal);
		}
	}

	void FMegaObjectFactory::GenTangents(
		PlatformVertex* vertices,
		uint32 numVertices,
		uint32* indices,
		uint32 numIndices) {

		for (uint32 i = 0; i < numIndices; i += 3) {
			glm::vec4 v0 = vertices[indices[i + 0]].position;
			glm::vec4 v1 = vertices[indices[i + 1]].position;
			glm::vec4 v2 = vertices[indices[i + 2]].position;

			// Shortcuts for UVs
			glm::vec4 uv0 = vertices[indices[i + 0]].uv;
			glm::vec4 uv1 = vertices[indices[i + 1]].uv;
			glm::vec4 uv2 = vertices[indices[i + 2]].uv;

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec4 tangent = glm::vec4((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r, 0.f);
			tangent = glm::normalize(tangent);

			vertices[indices[i + 0]].tangent += tangent;
			vertices[indices[i + 1]].tangent += tangent;
			vertices[indices[i + 2]].tangent += tangent;
		}

		for (uint32 i = 0; i < numVertices; i++) {
			auto& v = vertices[i];
			v.tangent = glm::normalize(v.tangent);

			if (glm::isnan(v.tangent) != glm::bvec4(0) || glm::abs(glm::length2(v.tangent) - 1.f) >= 1e-4f) {
				v.tangent = v.normal + glm::vec4(1, 1, 1, 0);
			}

			v.tangent = glm::normalize(v.tangent - glm::dot(v.tangent, v.normal) * v.normal);
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
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->PushToBufferFormat(MeshVariableType::FLOAT32, 4);
		m->FinishBuffer();

		return m;
	}

}