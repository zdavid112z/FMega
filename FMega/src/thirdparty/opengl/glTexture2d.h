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
		GLTexture2D(const Image& image, TextureFilter filter = TextureFilter::LINEAR, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE, bool generateMipmaps = true);
		~GLTexture2D();

		void Bind(uint32 slot);
	private:
		uint32 m_Width, m_Height;
		GLuint m_Texture;

	};

}