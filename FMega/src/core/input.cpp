#include "fmega.h"
#include "input.h"

namespace fmega {

	Input::Input(EventManager* eventManager)
	{
		eventManager->AddEventListener(EventType::KEY_DOWN,
			EventListener(
				[this](Event e)
		{
			KeyEventData* data = (KeyEventData*)e.data;
			//LOG_INFO("Key Pressed ", (uint32)data->key);
			m_KeyStates[(uint32)data->key] = true;
			m_KeyPressed[(uint32)data->key] = true;
			return false;
		}
			, 1000)
		);

		eventManager->AddEventListener(EventType::KEY_UP,
			EventListener(
				[this](Event e)
		{
			KeyEventData* data = (KeyEventData*)e.data;
			//LOG_INFO("Key Released ", (uint32)data->key);
			m_KeyStates[(uint32)data->key] = false;
			m_KeyReleased[(uint32)data->key] = true;
			return false;
		}
				, 1000)
		);

		eventManager->AddEventListener(EventType::MOUSE_BUTTON_DOWN,
			EventListener(
				[this](Event e)
		{
			MouseButtonEventData* data = (MouseButtonEventData*)e.data;
			//LOG_INFO("MB Pressed ", (uint32)data->mouseButton);
			m_MouseButtonStates[(uint32)data->mouseButton] = true;
			m_MouseButtonPressed[(uint32)data->mouseButton] = true;
			return false;
		}
				, 1000)
		);

		eventManager->AddEventListener(EventType::MOUSE_BUTTON_UP,
			EventListener(
				[this](Event e)
		{
			MouseButtonEventData* data = (MouseButtonEventData*)e.data;
			//LOG_INFO("MB Released ", (uint32)data->mouseButton);
			m_MouseButtonStates[(uint32)data->mouseButton] = false;
			m_MouseButtonReleased[(uint32)data->mouseButton] = true;
			return false;
		}
				, 1000)
		);

		eventManager->AddEventListener(EventType::MOUSE_MOTION,
			EventListener(
				[this](Event e)
		{
			MouseMotionEventData* data = (MouseMotionEventData*)e.data;
			//LOG_INFO("Mouse Moved ", data->offset);
			m_MousePosition = data->position;
			return false;
		}
				, 1000)
		);

		eventManager->AddEventListener(EventType::SHUTDOWN,
			EventListener(
				[this](Event e)
		{
			m_ShouldClose = true;
			return false;
		}
				, 1000)
		);

		m_ShouldClose = false;
		memset(m_KeyStates, 0, sizeof(m_KeyStates));
		memset(m_MouseButtonStates, 0, sizeof(m_MouseButtonStates));
		memset(m_KeyPressed, 0, sizeof(m_KeyPressed));
		memset(m_KeyReleased, 0, sizeof(m_KeyReleased));
		memset(m_MouseButtonPressed, 0, sizeof(m_MouseButtonPressed));
		memset(m_MouseButtonReleased, 0, sizeof(m_MouseButtonReleased));
	}

	Input::~Input()
	{

	}

	void Input::PostLogicUpdate()
	{
		memset(m_KeyPressed, 0, sizeof(m_KeyPressed));
		memset(m_KeyReleased, 0, sizeof(m_KeyReleased));
		memset(m_MouseButtonPressed, 0, sizeof(m_MouseButtonPressed));
		memset(m_MouseButtonReleased, 0, sizeof(m_MouseButtonReleased));
	}

	bool Input::WasKeyPressed(Key k)
	{
		return m_KeyPressed[(uint32)k];
	}

	bool Input::WasKeyReleased(Key k)
	{
		return m_KeyReleased[(uint32)k];
	}

	bool Input::WasMouseButtonPressed(MouseButton k)
	{
		return m_MouseButtonPressed[(uint32)k];
	}

	bool Input::WasMouseButtonReleased(MouseButton k)
	{
		return m_MouseButtonReleased[(uint32)k];
	}

	bool Input::IsKeyDown(Key k)
	{
		return m_KeyStates[(uint32)k];
	}

	bool Input::IsMouseButtonDown(MouseButton k)
	{
		return m_MouseButtonStates[(uint32)k];
	}

	bool Input::IsKeyUp(Key k)
	{
		return !m_KeyStates[(uint32)k];
	}

	bool Input::IsMouseButtonUp(MouseButton k)
	{
		return !m_MouseButtonStates[(uint32)k];
	}
	
	glm::vec2 Input::GetMousePosition()
	{
		return m_MousePosition;
	}

	bool Input::ShouldClose()
	{
		return m_ShouldClose;
	}

}