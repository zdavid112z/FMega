#pragma once

#include "fmega.h"

#ifdef FMEGA_OPENGL
#include "thirdparty/opengl/glMesh.h"
namespace fmega {
	typedef GLMesh Mesh;
}
#endif
