#pragma once

#if defined(FMEGA_WIN32) || defined(FMEGA_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#define FMEGA_WINDOWS
#define FMEGA_OPENGL
#define FMEGA_GLFW
#define FMEGA_STB_IMAGE
#define FMEGA_ASSIMP
#define FMEGA_NO_IMGUI
#define GLEW_STATIC
#endif

#ifdef FMEGA_STB_IMAGE
#define OPENGL_IMAGE_UNPACKED
#endif

#ifdef FMEGA_DEBUG
#define FMEGA_SHOW_LOGS
#define FMEGA_EVAL_ASSERT
#endif

#define FMEGA_MAX_COMPONENTS 65536
#define FMEGA_MAX_COMPONENTS_IN_SYSTEM 32

#define FMEGA_SHOW_LOGS