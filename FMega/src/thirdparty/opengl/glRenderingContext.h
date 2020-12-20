#pragma once

#include "fmega.h"
#include "opengl.h"
#include "graphics/renderingContextShared.h"

namespace fmega {

	class GLRenderingContext
	{
	public:
		GLRenderingContext();
		~GLRenderingContext();
		void SetDepthTest(bool test);
		void SetDepthMask(bool mask);
		void SetColorMask(bool mask);
		void SetColorMask(bool r, bool g, bool b, bool a);
		void SetStencilMask(uint32 mask);
		void SetStencilFunction(StencilFunction function, int value, int mask);
		void SetStencilOperation(StencilOperation fail, StencilOperation depthfail, StencilOperation success);
		void StencilWriteSimple(int value);
		// To use the replace operation use the other function
		void StencilWriteSimple(StencilOperation operation);
		void StencilPassIfSimple(int value, StencilFunction function = StencilFunction::EQUAL);
		void SetClearColor(glm::vec4 color);
		void Clear(ClearFlag flag);
		void SetImageUnpacked(bool unpacked);
		void SetScreenRenderTarget();
		void SetDebugOutput(bool debug);
		void PrintSystemSpecs();
		void SetCullFace(CullFace cullFace);
		void SetWireframe(bool wireframe);
	private:

	};

}
