#pragma once

#include "fmega.h"
#include "event.h"

namespace fmega {

	// Function returns true if you want to stop the event propagation
	struct EventListener
	{
		EventListener() {}
		EventListener(std::function<bool(Event)> function, float priority = 0)
			: priority(priority), function(function) {}
		// Reserved
		uint32 id = 0;
		float priority = 0;
		std::function<bool(Event)> function;
	};

}
