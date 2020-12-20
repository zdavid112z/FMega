#pragma once

namespace fmega {

	enum class TextureFilter
	{
		NEAREST,
		LINEAR
	};

	enum class TextureWrap
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE
	};

}