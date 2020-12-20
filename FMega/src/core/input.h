#pragma once

#include "fmega.h"
#include "event/eventManager.h"
#ifdef FMEGA_GLFW
#include "thirdparty/glfw/glfwInput.h"
#endif

namespace fmega {

	struct KeyEventData
	{
		Key key;
		uint32 mods;
	};

	struct MouseButtonEventData
	{
		MouseButton mouseButton;
		uint32 mods;
	};

	struct MouseMotionEventData
	{
		glm::vec2 offset;
		glm::vec2 position;
	};

	struct ScreenResizeEventData
	{
		glm::ivec2 resolution;
	};

	class Input
	{
	public:
		Input(EventManager* eventManager);
		~Input();

		bool IsKeyDown(Key k);
		bool IsMouseButtonDown(MouseButton k);
		bool IsKeyUp(Key k);
		bool IsMouseButtonUp(MouseButton k);

		bool WasKeyPressed(Key k);
		bool WasKeyReleased(Key k);

		bool WasMouseButtonPressed(MouseButton k);
		bool WasMouseButtonReleased(MouseButton k);

		void PostLogicUpdate();
		glm::vec2 GetMousePosition();
		bool ShouldClose();
	private:
		bool m_ShouldClose;
		glm::vec2 m_MousePosition;
		bool m_KeyStates[(uint32)Key::NUM_KEYS];
		bool m_KeyPressed[(uint32)Key::NUM_KEYS];
		bool m_KeyReleased[(uint32)Key::NUM_KEYS];
		bool m_MouseButtonStates[(uint32)MouseButton::NUM_BUTTONS];
		bool m_MouseButtonPressed[(uint32)MouseButton::NUM_BUTTONS];
		bool m_MouseButtonReleased[(uint32)MouseButton::NUM_BUTTONS];
	};

}
