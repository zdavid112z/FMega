#pragma once

#include "fmega.h"

namespace fmega {
	
	enum class ClearFlag : uint32
	{
		NONE = 0,
		COLOR = 1,
		DEPTH = 2,
		COLOR_DEPTH = 3,
		STENCIL = 4,
		COLOR_STENCIL = 5,
		DEPTH_STENCIL = 6,
		COLOR_DEPTH_STENCIL = 7
	};

	enum class StencilOperation : uint32
	{
		KEEP,
		MAKE_ZERO,
		REPLACE,
		INCREMENT,
		INCREMENT_WARP,
		DECREMENT,
		DECREMENT_WARP,
		INVERT
	};

	enum class StencilFunction : uint32
	{
		NEVER_PASS,
		ALWAYS_PASS,
		EQUAL,
		NOT_EQUAL,
		GREATER,
		LESS,
		GREATER_OR_EQUAL,
		LESS_OR_EQUAL
	};

	enum class CullFace : uint32
	{
		NONE,
		FRONT,
		BACK
	};

}