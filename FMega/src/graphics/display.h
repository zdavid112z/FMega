#pragma once

#include "fmega.h"

#ifdef FMEGA_GLFW
#include "thirdparty/glfw/glfwDisplay.h"
namespace fmega {
	typedef GLFWDisplay Display;
}
#endif