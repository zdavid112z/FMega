#pragma once

#include "fmega.h"

#ifdef FMEGA_OPENGL
#include "thirdparty/opengl/glTexture2d.h"
namespace fmega {
	typedef GLTexture2D Texture2D;
}
#endif