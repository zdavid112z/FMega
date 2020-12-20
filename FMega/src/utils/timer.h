#pragma once

#include "fmega.h"
#ifdef FMEGA_GLFW
#include "thirdparty/glfw/glfwTimer.h"
namespace fmega {
	typedef GLFWTimer Timer;
}
#endif