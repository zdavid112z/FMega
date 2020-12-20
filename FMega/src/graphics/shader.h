#pragma once

#include "fmega.h"

#ifdef FMEGA_OPENGL
#include "thirdparty/opengl/glShader.h"
namespace fmega {
	typedef GLShader Shader;
}
#endif
