#include "fmega.h"
#include "glShader.h"
#include "io/files.h"

namespace fmega {

	GLShader* GLShader::s_CurrentShader = nullptr;

	GLShader::GLShader(const std::string& vertPath, const std::string& fragPath, size_t dynamicObjectBufferSize) :
		m_DynamicObjectBufferSize(dynamicObjectBufferSize)
	{
		if (dynamicObjectBufferSize != 0)
			m_DynamicObjectBuffer = new GLBuffer(BufferType::UNIFORM, 0, dynamicObjectBufferSize, BufferUsage::DYNAMIC_DRAW);
		else m_DynamicObjectBuffer = nullptr;
		m_Program = glCreateProgram();
		std::string vertSrc = Files::ReadFile(vertPath);
		std::string fragSrc = Files::ReadFile(fragPath);
		vertSrc = PreprocessSource(vertSrc);
		fragSrc = PreprocessSource(fragSrc);
		GLuint vert = CreateShader(vertSrc, GL_VERTEX_SHADER, vertPath);
		GLuint frag = CreateShader(fragSrc, GL_FRAGMENT_SHADER, fragPath);
		if(vert)
		glAttachShader(m_Program, vert);
		glAttachShader(m_Program, frag);
		glLinkProgram(m_Program);

		GLint status;
		glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			char buffer[1024];
			glGetProgramInfoLog(m_Program, 1024, NULL, buffer);
			LOG_FATAL("Could not link program!");
			LOG_FATAL("Status: ", status);
			LOG_FATAL("Log:");
			LOG_FATAL(buffer);
			LOG_FATAL();
			LOG_FATAL("Vertex shader: '", vertPath, "'");
			LOG_FATAL("Fragment shader: '", fragPath, "'");
			LOG_FATAL();
			LOG_FATAL("Vertex shader source:");
			LOG_FATAL(vertSrc);
			LOG_FATAL();
			LOG_FATAL("Fragment shader source:");
			LOG_FATAL(fragSrc);
		}

		glDeleteShader(vert);
		glDeleteShader(frag);
	}

	GLShader::~GLShader()
	{
		glDeleteProgram(m_Program);
		if (m_DynamicObjectBuffer)
			delete m_DynamicObjectBuffer;
	}

	void GLShader::Bind()
	{
		//if (s_CurrentShader != this)
		{
			glUseProgram(m_Program);
			s_CurrentShader = this;
		}
	}

	void GLShader::SetBuffer(const std::string& name, GLBuffer* buffer)
	{
		Bind();
		auto it = m_Uniforms.find(name);
		GLuint binding;
		if (it == m_Uniforms.end())
		{
			GLUniform uniform;
			uniform.name = name;
			uniform.binding = m_UniformBlocks++;
			uniform.index = glGetUniformBlockIndex(m_Program, name.c_str());
			if (uniform.index == GL_INVALID_INDEX)
				LOG_ERROR("Could not find uniform block '", name, "'!");
			glUniformBlockBinding(m_Program, uniform.index, uniform.binding);
			uniform.buffer = buffer;
			m_Uniforms[name] = uniform;
			binding = uniform.binding;
		}
		else
		{
			binding = it->second.binding;
			it->second.buffer = buffer;
		}
		buffer->BindBase(binding);
	}

	void GLShader::SetBuffer(const std::string& name, GLBuffer* buffer, uint32 offset, uint32 size)
	{
		Bind();
		auto it = m_Uniforms.find(name);
		GLuint binding;
		if (it == m_Uniforms.end())
		{
			GLUniform uniform;
			uniform.name = name;
			uniform.binding = m_UniformBlocks++;
			uniform.index = glGetUniformBlockIndex(m_Program, name.c_str());
			if (uniform.index == GL_INVALID_INDEX)
				LOG_ERROR("Could not find uniform block '", name, "'!");
			glUniformBlockBinding(m_Program, uniform.index, uniform.binding);
			uniform.buffer = buffer;
			m_Uniforms[name] = uniform;
			binding = uniform.binding;
		}
		else
		{
			binding = it->second.binding;
			it->second.buffer = buffer;
		}
		buffer->BindRange(binding, offset, size);
	}

	void GLShader::SetTexture(const std::string& name, GLTexture2D* texture)
	{
		Bind();
		auto it = m_Uniforms.find(name);
		GLuint binding;
		if (it == m_Uniforms.end())
		{
			GLUniform uniform;
			uniform.name = name;
			uniform.binding = m_Textures++;
			uniform.index = glGetUniformLocation(m_Program, name.c_str());
			glUniform1i(uniform.index, uniform.binding);
			if (uniform.index == -1)
				LOG_ERROR("Could not find texture '", name, "'!");
			uniform.texture = texture;
			m_Uniforms[name] = uniform;
			binding = uniform.binding;
		}
		else
		{
			binding = it->second.binding;
			it->second.texture = texture;
		}
		texture->Bind(binding);
	}

	GLuint GLShader::CreateShader(const std::string& src, GLuint type, const std::string& path)
	{
		GLuint shader = glCreateShader(type);
		GLchar* s = (GLchar*)src.c_str();
		GLint l = src.length();
		glShaderSource(shader, 1, &s, NULL);
		glCompileShader(shader);
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			char buffer[1024];
			glGetShaderInfoLog(shader, 1024, NULL, buffer);
			LOG_FATAL("Could not compile shader from: '", path, "'!");
			LOG_FATAL("Status: ", status);
			LOG_FATAL("Log:");
			LOG_FATAL(buffer);
			LOG_FATAL();
			LOG_FATAL("Source code:");
			LOG_FATAL(src);
			LOG_FATAL();
		}
		return shader;
	}

	void GLShader::SetDynamicObjectBuffer(const std::string& name, byte* data)
	{
		m_DynamicObjectBuffer->SetSubdata(data, m_DynamicObjectBufferSize, 0);
		SetBuffer(name, m_DynamicObjectBuffer);
	}

	std::string GLShader::PreprocessSource(const std::string& src)
	{
		std::string result;
		size_t pos = 0, last = 0;
		while ((pos = src.find("#include", last)) != std::string::npos)
		{
			result += src.substr(last, pos - last);
			pos += 8; // move after #include
			// skip spaces
			while (pos < src.size() && (src[pos] == ' ' || src[pos] == '\t'))
				pos++;
			ASSERT(pos < src.size() && (src[pos] == '"' || src[pos] == '<'), "Could not preprocess a shader source! Expected '\"' or '<' after #include");
			pos++;
			size_t begin = pos; // of the included file
			while (pos < src.size() && src[pos] != '"' && src[pos] != '>')
				pos++; // skip until there is a closing symbol
			ASSERT(pos < src.size(), "Could not preprocess a shader source! Expected '\"' or '>' after #include <path>");
			// get the included file's name
			std::string includedFile = src.substr(begin, pos - begin);
			result += '\n'; // safety
			result += PreprocessSource(Files::ReadFile("assets/shaders/" + includedFile));
			result += '\n'; // safety
			// go after the closing symbol
			pos++;
			last = pos;
		}
		result += src.substr(last);
		return  result;
	}

}