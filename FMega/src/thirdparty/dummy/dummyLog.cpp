#include "fmega.h"
#if !defined(FMEGA_DEBUG) && defined(FMEGA_REMOVE_FORCE_LOG)
#include "utils/log.h"

namespace fmega {
	void PrintToConsoleInternal(const std::string& str) {}
	void SetLogWarningLevelInternal(LogWarningLevel level) {}
}

#endif