#include "fmega.h"
#include "glMesh.h"

namespace fmega {

	GLMesh::GLMesh(PrimitiveType pt, bool deleteBuffers, bool deleteIBO) :
		m_DeleteBuffers(deleteBuffers),
		m_DeleteIBO(deleteIBO),
		m_PrimitiveType(pt),
		m_DeleteMeshData(false),
		m_HasMeshData(false)
	{
		m_MeshData.vertices = nullptr;
		m_MeshData.indices = nullptr;
		m_MeshData.numIndices = m_MeshData.numVertices = 0;
		switch (m_PrimitiveType)
		{
		case PrimitiveType::TRIANGLES:
			m_GLPrimitiveType = GL_TRIANGLES;
			break;
		case PrimitiveType::TRIANGLE_STRIP:
			m_GLPrimitiveType = GL_TRIANGLE_STRIP;
			break;
		case PrimitiveType::LINES:
			m_GLPrimitiveType = GL_LINES;
			break;
		}
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
	}

	GLMesh::GLMesh(MeshData meshData, PrimitiveType primitiveType, bool deleteMeshData) :
		m_DeleteBuffers(true),
		m_DeleteIBO(true),
		m_PrimitiveType(primitiveType),
		m_DeleteMeshData(deleteMeshData),
		m_MeshData(meshData),
		m_HasMeshData(deleteMeshData)
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		switch (m_PrimitiveType)
		{
		case PrimitiveType::TRIANGLES:
			m_GLPrimitiveType = GL_TRIANGLES;
			break;
		case PrimitiveType::TRIANGLE_STRIP:
			m_GLPrimitiveType = GL_TRIANGLE_STRIP;
			break;
		case PrimitiveType::LINES:
			m_GLPrimitiveType = GL_LINES;
			break;
		}
		GLBuffer* ibo = new GLBuffer(BufferType::ELEMENT_ARRAY, (byte*)meshData.indices, meshData.numIndices * sizeof(uint32), BufferUsage::STATIC_DRAW);
		GLBuffer* vbo = new GLBuffer(BufferType::ARRAY, (byte*)meshData.vertices, meshData.numVertices * sizeof(Vertex), BufferUsage::STATIC_DRAW);
		SetIBO(ibo, meshData.numIndices);
		AddBuffer(vbo);
		PushToBufferFormat(MeshVariableType::FLOAT32, 3);
		PushToBufferFormat(MeshVariableType::FLOAT32, 2);
		PushToBufferFormat(MeshVariableType::FLOAT32, 3);
		PushToBufferFormat(MeshVariableType::FLOAT32, 3);
		PushToBufferFormat(MeshVariableType::FLOAT32, 3);
		FinishBuffer();
	}

	GLMesh::~GLMesh()
	{
		glDeleteVertexArrays(1, &m_VAO);
		if (m_DeleteBuffers)
			for (const GLBuffer* b : m_Buffers)
				delete b;
		if (m_DeleteIBO && m_IBO)
			delete m_IBO;
		if (m_DeleteMeshData)
		{
			delete m_MeshData.vertices;
			delete m_MeshData.indices;
		}
	}

	void GLMesh::DrawNow(uint32 numInstances)
	{
		glBindVertexArray(m_VAO);
		if (m_IBO)
		{
			m_IBO->Bind();
			glDrawElementsInstanced(m_GLPrimitiveType, m_NumIndices, GL_UNSIGNED_INT, nullptr, numInstances);
			// glDrawElements(m_GLPrimitiveType, m_NumIndices, GL_UNSIGNED_INT, nullptr);
		}
		else glDrawArraysInstanced(m_GLPrimitiveType, 0, m_NumIndices, numInstances);
	}

	void GLMesh::SetIBO(GLBuffer* ibo, uint32 numIndices, uint32 offset)
	{
		m_IBO = ibo;
		m_NumIndices = numIndices;
		m_IBOOffset = offset;
	}

	void GLMesh::AddBuffer(GLBuffer* buffer)
	{
		m_Buffers.push_back(buffer);
		m_FormatElements.clear();
		m_FormatSize = 0;
	}

	uint32 GLMesh::PushToBufferFormat(MeshVariableType varType, uint32 numComponents, bool normalized, uint32 divisor, uint32 index)
	{
		if (index == 0xffffffff)
			index = m_MeshIndex;

		GLFormatElement fe;
		fe.index = index;
		fe.normalized = normalized;
		fe.numComponents = numComponents;
		fe.divisor = divisor;
		GLuint types[] = { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT };
		uint32 sizes[] = { 1, 1, 2, 2, 4, 4, 4 };
		fe.type = types[(uint32)varType];
		fe.size = sizes[(uint32)varType];
		m_FormatElements.push_back(fe);

		m_FormatSize += fe.size * fe.numComponents;

		m_MeshIndex = index + 1;
		return index;
	}

	void GLMesh::FinishBuffer()
	{
		uint32 offset = 0;
		m_Buffers.back()->Bind();
		for (const GLFormatElement fe : m_FormatElements)
		{
			glEnableVertexAttribArray(fe.index);
			glVertexAttribDivisor(fe.index, fe.divisor);
			glVertexAttribPointer(fe.index, fe.size, fe.type, fe.normalized, m_FormatSize, (const void*)offset);
			offset += fe.size * fe.numComponents;
		}
		if (m_IBO == nullptr)
			m_NumIndices = m_Buffers.back()->GetSize() / m_FormatSize;
	}

}