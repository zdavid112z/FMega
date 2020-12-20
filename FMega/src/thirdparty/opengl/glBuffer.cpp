#include "fmega.h"
#include "glBuffer.h"

namespace fmega {

	GLBuffer* GLBuffer::s_CurrentBuffers[(uint32)BufferType::NUM_TYPES] = { nullptr, nullptr, nullptr };

	GLBuffer::GLBuffer(BufferType type)
	{
		m_Type = type;
		GLuint targets[] = { GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER };
		m_Target = targets[(uint32)type];
		glGenBuffers(1, &m_Buffer);
	}

	GLBuffer::GLBuffer(BufferType type, byte* data, uint32 dataSize, BufferUsage usage)
	{
		m_Type = type;
		GLuint targets[] = { GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER };
		m_Target = targets[(uint32)type];
		glGenBuffers(1, &m_Buffer);
		SetData(data, dataSize, usage);
	}

	GLBuffer::~GLBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void GLBuffer::Bind()
	{
		// if (s_CurrentBuffers[(uint32)m_Type] != this)
		{
			glBindBuffer(m_Target, m_Buffer);
			// s_CurrentBuffers[(uint32)m_Type] = this;
		}
	}

	void GLBuffer::BindBase(uint32 index)
	{
		glBindBufferBase(m_Target, index, m_Buffer);
	}

	void GLBuffer::BindRange(uint32 index, uint32 offset, uint32 size)
	{
		glBindBufferRange(m_Target, index, m_Buffer, offset, size);
	}

	byte* GLBuffer::Map(BufferAccessType accessType)
	{
		GLenum access[] = { GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE };
		Bind();
		return (byte*)glMapBuffer(m_Target, access[(uint32)accessType]);
	}

	byte* GLBuffer::MapRange(BufferAccessType accessType, uint32 dataSize, uint32 offset)
	{
		GLenum access[] = { GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE };
		Bind();
		return (byte*)glMapBufferRange(m_Target, offset, dataSize, access[(uint32)accessType]);
	}

	void GLBuffer::Unmap()
	{
		Bind();
		glUnmapBuffer(m_Target);
	}

	void GLBuffer::SetData(byte* data, uint32 dataSize, BufferUsage usage)
	{
		m_Size = dataSize;
		GLuint usages[] = { GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_READ, GL_STATIC_READ, GL_DYNAMIC_READ, GL_STREAM_COPY, GL_STATIC_COPY, GL_DYNAMIC_COPY };
		GLuint bufferUsage = usages[(uint32)usage];
		Bind();
		glBufferData(m_Target, dataSize, (void*)data, bufferUsage);
	}

	void GLBuffer::SetSubdata(byte* data, uint32 dataSize, uint32 offset)
	{
		Bind();
		glBufferSubData(m_Target, offset, dataSize, data);
	}

}