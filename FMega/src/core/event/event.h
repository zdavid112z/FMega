#pragma once

#include "eventType.h"

namespace fmega {

	struct Event
	{
		union
		{
			void* data;
			uint32* idata;
			float* fdata;
		};
		const char* message;
		uint32 type;
	};

}
