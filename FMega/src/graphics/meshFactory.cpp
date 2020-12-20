#include "fmega.h"
#include "meshFactory.h"

namespace fmega {

	void MeshFactory::CreateCube(Vertex* v, uint32* ind, float length)
	{
		float l2 = length / 2.f;

		glm::vec3 vertices[8] =
		{
			glm::vec3(-l2, -l2, -l2),
			glm::vec3(l2, -l2, -l2),
			glm::vec3(l2, l2, -l2),
			glm::vec3(-l2, l2, -l2),
			glm::vec3(-l2, -l2, l2),
			glm::vec3(l2, -l2, l2),
			glm::vec3(l2, l2, l2),
			glm::vec3(-l2, l2, l2)
		};

		glm::vec2 texCoords[4] =
		{
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(1, 1),
			glm::vec2(0, 1)
		};

		glm::vec3 normals[6] =
		{
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, -1),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, -1, 0)
		};

		int indices[6 * 6] =
		{
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		int texInds[6] = { 0, 1, 3, 3, 1, 2 };

		for (int i = 0; i < 36; i++) {
			v[i].position = vertices[indices[i]];
			v[i].texCoord = texCoords[texInds[i % 4]];
			v[i].normal = normals[indices[i / 6]];
			ind[i] = i;
		}
		CalculateTangentsAndBitangents(v, 36, ind, 36);
	}

	void MeshFactory::CalculateTangentsAndBitangents(Vertex* v, uint32 numVertices, uint32* indices, uint32 numIndices)
	{
		for (uint32 i = 0; i < numVertices; i++)
			v[i].tangent = v[i].bitangent = glm::vec3(0, 0, 0);
		for (uint32 i = 0; i < numIndices; i += 3)
		{
			int i1 = i;
			int i2 = i + 1;
			int i3 = i + 2;

			const glm::vec3& v1 = v[i1].position;
			const glm::vec3& v2 = v[i2].position;
			const glm::vec3& v3 = v[i3].position;

			const glm::vec2& w1 = v[i1].texCoord;
			const glm::vec2& w2 = v[i2].texCoord;
			const glm::vec2& w3 = v[i3].texCoord;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			v[i1].tangent += sdir;
			v[i2].tangent += sdir;
			v[i3].tangent += sdir;

			v[i1].bitangent += tdir;
			v[i2].bitangent += tdir;
			v[i3].bitangent += tdir;
		}

		for (uint32 i = 0; i < numVertices; i++)
		{
			const glm::vec3& n = v[i].normal;
			const glm::vec3& t = v[i].tangent;

			v[i].tangent = glm::normalize((t - n * glm::dot(n, t)));
			float m = (glm::dot(glm::cross(n, t), v[i].bitangent) < 0.0F) ? -1.f : 1.f;
			v[i].bitangent = glm::cross(n, t) * m;
		}
	}

}