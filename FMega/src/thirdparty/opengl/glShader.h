#pragma once

#include "fmega.h"
#include "opengl.h"
#include "glBuffer.h"
#include "glTexture2d.h"
#include "glRenderingContext.h"

namespace fmega {

	class GLShader
	{
	public:
		static GLShader* s_CurrentShader;
	public:
		GLShader(const std::string& vertPath, const std::string& fragPath, size_t dynamicObjectBufferSize = 0);
		~GLShader();
		void Bind();

		void SetBuffer(const std::string& name, GLBuffer* buffer);
		void SetBuffer(const std::string& name, GLBuffer* buffer, uint32 offset, uint32 size);
		void SetTexture(const std::string& name, GLTexture2D* texture);
		void SetDynamicObjectBuffer(const std::string& name, byte* data);
	private:
		GLuint CreateShader(const std::string& src, GLuint type, const std::string& path = "Unknown");
		std::string PreprocessSource(const std::string& src);
	private:
		struct GLUniform
		{
			std::string name;
			GLint index = 0;
			GLuint binding = 0;
			union
			{
				GLTexture2D* texture = nullptr;
				GLBuffer* buffer;
			};
		};
		std::unordered_map<std::string, GLUniform> m_Uniforms;
		uint32 m_UniformBlocks = 0;
		uint32 m_Textures = 0;
		GLuint m_Program;
		size_t m_DynamicObjectBufferSize;
		GLBuffer* m_DynamicObjectBuffer;
	};

}
