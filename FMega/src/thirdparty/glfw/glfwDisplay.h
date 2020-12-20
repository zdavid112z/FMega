#pragma once

#include "fmega.h"
#ifdef FMEGA_GLFW

#include "graphics/displayShared.h"
#include "opengl.h"
#include "utils/timer.h"
#include "core/input.h"
#include "core/event/eventManager.h"

#include "graphics/renderingContext.h"

namespace fmega {

	class GLFWDisplay
	{
	public:
		GLFWDisplay(const DisplayData& data, EventManager* eventManager);
		~GLFWDisplay();
		void Present();
		void PollEvents();
		void SetCursorGrabbed(bool);
		bool IsCursorGrabbed();

		uint32 GetWidth() { return m_Width; }
		uint32 GetHeight() { return m_Height; }
		float GetAspect() { return float(m_Width) / m_Height; }
		Input* GetInput() { return m_Input; }
		EventManager* GetEventManager() { return m_EventManager; }
		RenderingContext* GetRenderingContext() { return m_RenderingContext; }
	private:
		static void KeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void MousePosCallback(GLFWwindow* window, double x, double y);
		static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	private:
		RenderingContext* m_RenderingContext;
		EventManager* m_EventManager;
		Input* m_Input;
		uint32 m_Width, m_Height;
		GLFWwindow* m_Window;
		float m_FPSInterval;
		float m_FPSLastUpdated;
		Timer m_FPSTimer;
		uint32 m_FPSTarget;
	};

}

#endif