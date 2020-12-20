#pragma once

#include "fmega.h"

namespace fmega {

	class EventType
	{
	public:
		static uint32 KEY_DOWN;
		static uint32 KEY_UP;
		static uint32 MOUSE_MOTION;
		static uint32 MOUSE_BUTTON_DOWN;
		static uint32 MOUSE_BUTTON_UP;
		static uint32 RESIZE;
		static uint32 SHUTDOWN;
	};

}
