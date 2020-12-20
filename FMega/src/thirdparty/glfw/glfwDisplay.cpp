#include "fmega.h"
#ifdef FMEGA_GLFW
#include "glfwDisplay.h"
//#define FMEGA_NO_IMGUI

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

namespace fmega {

	GLFWDisplay::GLFWDisplay(const DisplayData& data, EventManager* eventManager)
	{
		m_EventManager = eventManager;
		int res = glfwInit();
		ASSERT(res == GLFW_TRUE, "GLFW failed to initialize!");
		m_Width = data.width;
		m_Height = data.height;
		GLFWmonitor* monitor = NULL;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		if (data.fullscreen)
			monitor = glfwGetPrimaryMonitor();
		m_Window = glfwCreateWindow(data.width, data.height, data.title.c_str(), monitor, NULL);
		ASSERT(m_Window != NULL, "Could not create GLFW window!");
		glfwSetWindowUserPointer(m_Window, this);
		glfwMakeContextCurrent(m_Window);
		res = glewInit();
		ASSERT(res == GLEW_OK, "GLEW failed to initialize!");
		m_RenderingContext = new RenderingContext();
#ifdef FMEGA_DEBUG
		m_RenderingContext->PrintSystemSpecs();
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
		SetCursorGrabbed(false);
		glfwSwapInterval(data.swapInterval);
		glfwSetKeyCallback(m_Window, KeyCallback);
		glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
		glfwSetCursorPosCallback(m_Window, MousePosCallback);
		glfwSetWindowSizeCallback(m_Window, WindowSizeCallback);

		m_Input = new Input(eventManager);

		m_FPSLastUpdated = m_FPSTimer.GetAbsoluteTime();
		m_FPSTarget = data.fpsLimit;
		m_FPSInterval = 1.f / data.fpsLimit;

		glEnable(GL_MULTISAMPLE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		SetCursorGrabbed(false);
	}

	void GLFWDisplay::SetCursorGrabbed(bool b)
	{
		if(b)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	bool GLFWDisplay::IsCursorGrabbed()
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	}

	GLFWDisplay::~GLFWDisplay()
	{
		delete m_RenderingContext;
		delete m_Input;
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWDisplay::Present()
	{
		if (m_FPSTarget != 0)
		{
			float now;
			while ((now = m_FPSTimer.GetAbsoluteTime()) - m_FPSLastUpdated < m_FPSInterval);
			int framesSkipped = int((now - m_FPSLastUpdated) / m_FPSInterval);
			if (framesSkipped > 1)
			{
				//LOG_WARN("Skipped ", framesSkipped - 1, " frames");
				m_FPSLastUpdated = now;
			}
			else m_FPSLastUpdated += m_FPSInterval;
		}
		glfwSwapBuffers(m_Window);
	}

	void GLFWDisplay::PollEvents()
	{
		glfwPollEvents();
		if (glfwWindowShouldClose(m_Window))
		{
			Event event;
			event.type = EventType::SHUTDOWN;
			m_EventManager->RegisterEvent(event);
		}
	}

	void GLFWDisplay::KeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
	{
		GLFWDisplay* display = (GLFWDisplay*)glfwGetWindowUserPointer(window);
		KeyEventData data;
		data.key = (Key)keycode;
		data.mods = mods;
		Event event;
		event.data = &data;
		if(action == GLFW_PRESS)
			event.type = EventType::KEY_DOWN;
		else if(action == GLFW_RELEASE)
			event.type = EventType::KEY_UP;
		if(action != GLFW_REPEAT)
			display->m_EventManager->RegisterEvent(event);
	}

	void GLFWDisplay::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		GLFWDisplay* display = (GLFWDisplay*)glfwGetWindowUserPointer(window);
		MouseButtonEventData data;
		data.mouseButton = (MouseButton)button;
;		data.mods = mods;
		Event event;
		event.data = &data;
		if (action == GLFW_PRESS)
			event.type = EventType::MOUSE_BUTTON_DOWN;
		else if (action == GLFW_RELEASE)
			event.type = EventType::MOUSE_BUTTON_UP;
		if (action != GLFW_REPEAT)
			display->m_EventManager->RegisterEvent(event);
	}

	void GLFWDisplay::MousePosCallback(GLFWwindow* window, double x, double y)
	{
		GLFWDisplay* display = (GLFWDisplay*)glfwGetWindowUserPointer(window);
		MouseMotionEventData data;
		static glm::vec2 offset = glm::vec2(x, y);
		data.position.x = float(x);
		data.position.y = float(y);
		data.offset = data.position - offset;
		offset = data.position;
		data.offset.x /= (float)display->GetWidth();
		data.offset.y /= (float)display->GetHeight();
		data.position.x /= (float)display->GetWidth();
		data.position.y /= (float)display->GetHeight();
		Event event;
		event.data = &data;
		event.type = EventType::MOUSE_MOTION;
		display->m_EventManager->RegisterEvent(event);
	}

	void GLFWDisplay::WindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		GLFWDisplay* display = (GLFWDisplay*)glfwGetWindowUserPointer(window);
		display->m_Width = width;
		display->m_Height = height;
		glViewport(0, 0, width, height);

		ScreenResizeEventData data;
		data.resolution.x = width;
		data.resolution.y = height;
		Event event;
		event.data = &data;
		event.type = EventType::RESIZE;
		display->m_EventManager->RegisterEvent(event);
	}

}

#endif