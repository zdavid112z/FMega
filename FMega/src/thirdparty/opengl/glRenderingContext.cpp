#include "fmega.h"
#include "glRenderingContext.h"

namespace fmega {

	GLRenderingContext::GLRenderingContext()
	{
		SetDepthTest(true);
		SetDepthMask(true);
		SetCullFace(CullFace::BACK);
		SetClearColor(glm::vec4(0.15f, 0.25f, 0.85f, 1));
		Clear(ClearFlag::COLOR_DEPTH_STENCIL);
#ifdef OPENGL_IMAGE_UNPACKED
		SetImageUnpacked(true);
#endif
#ifndef FMEGA_PRODUCTION
		SetDebugOutput(true);
#endif
	}

	GLRenderingContext::~GLRenderingContext()
	{

	}

	void GLRenderingContext::SetDepthTest(bool test)
	{
		if(test)
			glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	void GLRenderingContext::SetDepthMask(bool mask)
	{
		glDepthMask(mask);
	}

	void GLRenderingContext::SetColorMask(bool mask)
	{
		glColorMask(mask, mask, mask, mask);
	}

	void GLRenderingContext::SetColorMask(bool r, bool g, bool b, bool a)
	{
		glColorMask(r, g, b, a);
	}

	void GLRenderingContext::SetStencilMask(uint32 mask)
	{
		glStencilMask(mask);
	}

	void GLRenderingContext::StencilWriteSimple(int value)
	{
		SetStencilMask(0xff);
		SetStencilFunction(StencilFunction::ALWAYS_PASS, value, 0xff);
		SetStencilOperation(StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::REPLACE);
	}

	void GLRenderingContext::StencilWriteSimple(StencilOperation operation)
	{
		SetStencilMask(0xff);
		SetStencilOperation(StencilOperation::KEEP, StencilOperation::KEEP, operation);
	}

	void GLRenderingContext::StencilPassIfSimple(int value, StencilFunction function)
	{
		SetStencilMask(0);
		SetStencilFunction(function, value, 0xff);
	}

	void GLRenderingContext::SetStencilFunction(StencilFunction function, int value, int mask)
	{
		GLenum func = 0;
		switch (function)
		{
		case StencilFunction::ALWAYS_PASS:
			func = GL_ALWAYS;
			break;
		case StencilFunction::NEVER_PASS:
			func = GL_NEVER;
			break;
		case StencilFunction::EQUAL:
			func = GL_EQUAL;
			break;
		case StencilFunction::NOT_EQUAL:
			func = GL_NOTEQUAL;
			break;
		case StencilFunction::GREATER:
			func = GL_GREATER;
			break;
		case StencilFunction::LESS:
			func = GL_LESS;
			break;
		case StencilFunction::GREATER_OR_EQUAL:
			func = GL_GEQUAL;
			break;
		case StencilFunction::LESS_OR_EQUAL:
			func = GL_LEQUAL;
			break;
		}
		glStencilFunc(func, value, mask);
	}

	void GLRenderingContext::SetStencilOperation(StencilOperation fail, StencilOperation depthfail, StencilOperation success)
	{
		GLenum glv[3] = { 0, 0, 0 };
		StencilOperation val[3] = { fail, depthfail, success };
		for (int i = 0; i < 3; i++)
		{
			switch (val[i])
			{
			case StencilOperation::KEEP:
				glv[i] = GL_KEEP;
				break;
			case StencilOperation::REPLACE:
				glv[i] = GL_REPLACE;
				break;
			case StencilOperation::MAKE_ZERO:
				glv[i] = GL_ZERO;
				break;
			case StencilOperation::INCREMENT:
				glv[i] = GL_INCR;
				break;
			case StencilOperation::INCREMENT_WARP:
				glv[i] = GL_INCR_WRAP;
				break;
			case StencilOperation::DECREMENT:
				glv[i] = GL_DECR;
				break;
			case StencilOperation::DECREMENT_WARP:
				glv[i] = GL_DECR_WRAP;
				break;
			case StencilOperation::INVERT:
				glv[i] = GL_INVERT;
				break;
			default:
				return;
			}
		}
		glStencilOp(glv[0], glv[1], glv[2]);
	}

	void GLRenderingContext::SetClearColor(glm::vec4 color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void GLRenderingContext::Clear(ClearFlag flags)
	{
		uint32 glflags = 0;
		if ((uint32)flags & (uint32)ClearFlag::COLOR)
			glflags |= GL_COLOR_BUFFER_BIT;
		if ((uint32)flags & (uint32)ClearFlag::DEPTH)
			glflags |= GL_DEPTH_BUFFER_BIT;
		if ((uint32)flags & (uint32)ClearFlag::STENCIL)
			glflags |= GL_STENCIL_BUFFER_BIT;
		glClear(glflags);
	}

	void GLRenderingContext::SetImageUnpacked(bool unpacked)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	void GLRenderingContext::SetScreenRenderTarget()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void GLRenderingContext::SetCullFace(CullFace cullFace)
	{
		if (cullFace == CullFace::NONE)
			glDisable(GL_CULL_FACE);
		else
		{
			glEnable(GL_CULL_FACE);
			if (cullFace == CullFace::FRONT)
				glCullFace(GL_FRONT);
			else glCullFace(GL_BACK);
		}
	}

	void GLRenderingContext::SetWireframe(bool wireframe)
	{
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void GLAPIENTRY GLMessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		std::string ssource, stype;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			ssource = "DEBUG_SOURCE_API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			ssource = "DEBUG_SOURCE_WINDOW_SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			ssource = "DEBUG_SOURCE_SHADER_COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			ssource = "DEBUG_SOURCE_THIRD_PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			ssource = "DEBUG_SOURCE_APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			ssource = "DEBUG_SOURCE_OTHER";
			break;
		}
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			stype = "DEBUG_TYPE_ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			stype = "DEBUG_TYPE_DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			stype = "DEBUG_TYPE_UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			stype = "DEBUG_TYPE_PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			stype = "DEBUG_TYPE_PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_MARKER:
			stype = "DEBUG_TYPE_MARKER";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			stype = "DEBUG_TYPE_PUSH_GROUP";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			stype = "DEBUG_TYPE_POP_GROUP";
			break;
		case GL_DEBUG_TYPE_OTHER:
			stype = "DEBUG_TYPE_OTHER";
			break;
		}
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			LOG_VERB("GL Notification:");
			break;
		case GL_DEBUG_SEVERITY_LOW:
			LOG_INFO("GL Info:");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			LOG_WARN("GL Warning:");
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			LOG_ERROR("GL Error:");
			break;
		}
		Log::Print("Source: ", ssource, "; Type: ", stype, "; ID: ", id, '\n');
		Log::Print(message, "\n\n");
	}

	void GLRenderingContext::SetDebugOutput(bool debug)
	{
		if (debug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(GLMessageCallback, 0);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
		else glDisable(GL_DEBUG_OUTPUT);
	}

	void GLRenderingContext::PrintSystemSpecs()
	{
		int numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		LOG_VERB();
		LOG_VERB("Extensions: ", numExtensions);
		for (int i = 0; i < numExtensions; i++)
			LOG_VERB((const char*)glGetStringi(GL_EXTENSIONS, i));

		LOG_INFO();
		LOG_INFO("System specs:");
		LOG_INFO("--------------------------------------");
		LOG_INFO();
		LOG_INFO("Renderer: ", (const char*)glGetString(GL_RENDERER));
		LOG_INFO("Vendor:   ", (const char*)glGetString(GL_VENDOR));

#ifdef GL_NVX_gpu_memory_info
		GLint nTotalMemoryInKB = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
			&nTotalMemoryInKB);

		GLint nCurAvailMemoryInKB = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
			&nCurAvailMemoryInKB);

		int nTotalMemoryInMB = nTotalMemoryInKB / 1024;
		int nTotalMemoryInGB = nTotalMemoryInMB / 1024;

		int nCurAvailMemoryInMB = nCurAvailMemoryInKB / 1024;
		int nCurAvailMemoryInGB = nCurAvailMemoryInMB / 1024;

		LOG_INFO("Total memory: ", nTotalMemoryInGB, ".", nTotalMemoryInMB % 1024, " GB");
		LOG_INFO("Available memory: ", nCurAvailMemoryInGB, ".", nCurAvailMemoryInMB % 1024, " GB");

#elif defined(GL_ATI_meminfo)

		GLuint uNoOfGPUs = wglGetGPUIDsAMD(0, 0);
		GLuint* uGPUIDs = new GLuint[uNoOfGPUs];
		wglGetGPUIDsAMD(uNoOfGPUs, uGPUIDs);

		GLuint uTotalMemoryInMB = 0;
		wglGetGPUInfoAMD(uGPUIDs[0],
			WGL_GPU_RAM_AMD,
			GL_UNSIGNED_INT,
			sizeof(GLuint),
			&uTotalMemoryInMB);

		GLint nCurAvailMemoryInKB = 0;
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,
			&nCurAvailMemoryInKB);

		int nTotalMemoryInGB = nTotalMemoryInMB / 1024;

		int nCurAvailMemoryInMB = nCurAvailMemoryInKB / 1024;
		int nCurAvailMemoryInGB = nCurAvailMemoryInMB / 1024;

		//Debug::Printer << TLOG << "Total memory: " << nTotalMemoryInGB << "," << nTotalMemoryInMB % 1024 << " GB" << ENDL;
		//Debug::Printer << TLOG << "Available memory: " << nCurAvailMemoryInGB << "," << nCurAvailMemoryInMB % 1024 << " GB" << ENDL;
		LOG_INFO("Total memory: ", nTotalMemoryInGB, ".", nTotalMemoryInMB % 1024, " GB");
		LOG_INFO("Available memory: ", nCurAvailMemoryInGB, ".", nCurAvailMemoryInMB % 1024, " GB");

#endif

		LOG_INFO("OpenGL Version: ", (const char*)glGetString(GL_VERSION));
		LOG_INFO("Shading language version: ", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		LOG_INFO();
	}

}