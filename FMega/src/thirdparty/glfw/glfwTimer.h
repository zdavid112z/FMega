#pragma once

#include "fmega.h"
#ifdef FMEGA_GLFW
#include "opengl.h"

namespace fmega {

	class GLFWTimer
	{
	public:
		GLFWTimer() { Reset(); }
		void Reset() { m_Delta = 0; m_LastUpdate = glfwGetTime(); }
		float GetDelta() { return m_Delta; }
		void Update()
		{
			double now = glfwGetTime();
			m_Delta = float(now - m_LastUpdate);
			m_LastUpdate = now;
		}
		float GetAbsoluteTime() { return float(glfwGetTime()); }
	private:
		float m_Delta;
		double m_LastUpdate;
	};

}
#endif
