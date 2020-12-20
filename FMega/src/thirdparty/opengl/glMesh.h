#pragma once

#include "fmega.h"
#include "glBuffer.h"
#include "graphics/meshShared.h"
#include "glRenderingContext.h"

namespace fmega {

	class GLMesh
	{
	public:
		GLMesh(PrimitiveType pt = PrimitiveType::TRIANGLES, bool deleteBuffers = true, bool deleteIBO = true);
		GLMesh(MeshData data, PrimitiveType primitiveType = PrimitiveType::TRIANGLES, bool deleteMeshData = true);
		~GLMesh();

		void DrawNow(uint32 numInstances = 1);

		void SetIBO(GLBuffer* ibo, uint32 numIndices, uint32 offset = 0);
		void AddBuffer(GLBuffer* buffer);
		uint32 PushToBufferFormat(MeshVariableType varType, uint32 numComponents, bool normalized = false, uint32 divisor = 0, uint32 index = 0xffffffff);
		void FinishBuffer();

		GLBuffer* GetIBO() { return m_IBO; }
		GLBuffer* GetVBO(uint32 index) { return m_Buffers[index]; }
		uint32 GetNumVBOs() { return m_Buffers.size(); }
		MeshData GetMeshData() { return m_MeshData; }
		bool HasMeshData() { return m_HasMeshData; }
	private:
		struct GLFormatElement
		{
			GLenum type;
			uint32 size, numComponents, index, divisor;
			bool normalized;
		};

		GLenum m_GLPrimitiveType;
		GLuint m_VAO = 0;
		GLBuffer *m_IBO = nullptr;
		uint32 m_NumIndices = 0;
		uint32 m_IBOOffset;
		uint32 m_FormatSize = 0;
		std::vector<GLBuffer*> m_Buffers;
		std::vector<GLFormatElement> m_FormatElements;
		uint32 m_MeshIndex = 0;
		bool m_DeleteBuffers, m_DeleteIBO;
		PrimitiveType m_PrimitiveType;
		bool m_DeleteMeshData;
		bool m_HasMeshData;
		MeshData m_MeshData;
	};

}