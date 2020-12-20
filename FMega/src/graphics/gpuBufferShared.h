#pragma once

#include "fmega.h"

namespace fmega {
	
	enum class BufferType : uint32
	{
		ARRAY,
		ELEMENT_ARRAY,
		UNIFORM,
		NUM_TYPES
	};

	enum class BufferUsage : uint32
	{
		STREAM_DRAW,
		STATIC_DRAW,
		DYNAMIC_DRAW,
		STREAM_READ,
		STATIC_READ,
		DYNAMIC_READ,
		STREAM_COPY,
		STATIC_COPY,
		DYNAMIC_COPY
	};

	enum class BufferAccessType : uint32
	{
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE
	};

}