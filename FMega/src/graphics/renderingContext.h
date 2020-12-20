#pragma once

#include "fmega.h"

#ifdef FMEGA_OPENGL
#include "thirdparty/opengl/glRenderingContext.h"
namespace fmega {
	typedef GLRenderingContext RenderingContext;
}
#endif