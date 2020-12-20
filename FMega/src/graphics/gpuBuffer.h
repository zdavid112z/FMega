#pragma once

#include "fmega.h"

#ifdef FMEGA_OPENGL
#include "thirdparty/opengl/glBuffer.h"
namespace fmega {
	typedef GLBuffer GPUBuffer;
}
#endif