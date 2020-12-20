#pragma once

#include "fmega.h"
#include "opengl.h"
#include "graphics/gpuBufferShared.h"
#include "glRenderingContext.h"

namespace fmega {

	class GLRenderingContext;

	class GLBuffer
	{
		friend class GLRenderingContext;
	public:
		static GLBuffer* s_CurrentBuffers[(uint32)BufferType::NUM_TYPES];
	public:
		GLBuffer(BufferType type);
		GLBuffer(BufferType type, byte* data, uint32 dataSize, BufferUsage usage);
		~GLBuffer();
		void Bind();
		void BindBase(uint32 index);
		void BindRange(uint32 index, uint32 offset, uint32 size);
		byte* Map(BufferAccessType accessType);
		byte* MapRange(BufferAccessType accessType, uint32 dataSize, uint32 offset);
		void Unmap();
		void SetData(byte* data, uint32 dataSize, BufferUsage usage);
		void SetSubdata(byte* data, uint32 dataSize, uint32 offset);
		uint32 GetSize() { return m_Size; }
	private:
		BufferType m_Type;
		GLuint m_Target;
		GLuint m_Buffer;
		uint32 m_Size = 0;
	};

}