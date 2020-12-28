#pragma once

#include "fmega.h"
#include "opengl.h"
#include "graphics/texture2dShared.h"
#include "io/image.h"
#include "glRenderingContext.h"

namespace fmega {

	class GLTexture2D
	{
	public:
		GLTexture2D(const std::string& path, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE, bool generateMipmaps = true);
		GLTexture2D(const std::string& path, int32 numComponents, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE, bool generateMipmaps = true);
		GLTexture2D(const Image& image, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE, bool generateMipmaps = true);
		~GLTexture2D();

		void Bind(uint32 slot);
	private:
		void Init(const Image& image, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE, bool generateMipmaps = true);
	private:
		uint32 m_Width, m_Height;
		GLuint m_Texture;

	};

}