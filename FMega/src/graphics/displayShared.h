#pragma once

#include "fmega.h"

namespace fmega {

	struct DisplayData
	{
		uint32 width, height;
		std::string title = "App Title Here";
		bool fullscreen = false;
		uint32 swapInterval = 1;
		uint32 fpsLimit = 0;
	};

}