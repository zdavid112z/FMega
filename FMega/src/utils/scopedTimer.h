#pragma once

#include "timer.h"

namespace fmega {

	class ScopedTimer
	{
	public:
		ScopedTimer(const std::string& opName = "Operation") : m_Name(opName) { }
		~ScopedTimer()
		{
			m_Timer.Update();
			FORCE_LOG_INFO(m_Name, " took ", m_Timer.GetDelta(), "s");
		}
	private:
		std::string m_Name;
		Timer m_Timer;
	};

}