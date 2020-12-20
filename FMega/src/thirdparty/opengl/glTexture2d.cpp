#include "fmega.h"
#include "glTexture2d.h"

namespace fmega {

	GLTexture2D::GLTexture2D(const Image& image, TextureFilter filter, TextureWrap wrap, bool generateMipmaps)
	{
		glGenTextures(1, &m_Texture);
		GLuint glminfilter, glmagfilter, glwrap;
		if (generateMipmaps)
			glminfilter = filter == TextureFilter::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
		else glminfilter = filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST;
		glmagfilter = filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST;
		switch (wrap)
		{
		case TextureWrap::CLAMP_TO_EDGE:
			glwrap = GL_CLAMP_TO_EDGE;
			break;
		case TextureWrap::REPEAT:
			glwrap = GL_REPEAT;
			break;
		case TextureWrap::MIRRORED_REPEAT:
			glwrap = GL_MIRRORED_REPEAT;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glwrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glwrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glminfilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glmagfilter);
		GLuint iformat, format, type;
		switch (image.numComponents)
		{
		case 1:
			format = GL_RED;
			switch (image.bitsPerComponent)
			{
			case 8:
				iformat = GL_R8;
				break;
			case 16:
				iformat = GL_R16;
				break;
			case 32:
				iformat = GL_R32F;
				break;
			}
			break;
		case 2:
			format = GL_RG;
			switch (image.bitsPerComponent)
			{
			case 8:
				iformat = GL_RG8;
				break;
			case 16:
				iformat = GL_RG16;
				break;
			case 32:
				iformat = GL_RG32F;
				break;
			}
			break;
		case 3:
			format = GL_RGB;
			switch (image.bitsPerComponent)
			{
			case 8:
				iformat = GL_RGB8;
				break;
			case 16:
				iformat = GL_RGB16;
				break;
			case 32:
				iformat = GL_RGB32F;
				break;
			}
			break;
		case 4:
			format = GL_RGBA;
			switch (image.bitsPerComponent)
			{
			case 8:
				iformat = GL_RGBA8;
				break;
			case 16:
				iformat = GL_RGBA16;
				break;
			case 32:
				iformat = GL_RGBA32F;
				break;
			}
			break;
		}
		switch (image.bitsPerComponent)
		{
		case 8:
			type = GL_UNSIGNED_BYTE;
			break;
		case 16:
			type = GL_UNSIGNED_SHORT;
			break;
		case 32:
			type = GL_FLOAT;
			break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, iformat, image.width, image.height, 0, format, type, image.data);
		if (generateMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	GLTexture2D::~GLTexture2D()
	{
		glDeleteTextures(1, &m_Texture);
	}

	void GLTexture2D::Bind(uint32 slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
	}

}