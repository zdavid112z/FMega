#include "fmega.h"
#include "eventManager.h"
#include "eventType.h"

namespace fmega {

	EventManager::EventManager()
	{
		EventType::KEY_DOWN = RegisterEventType("KEY_DOWN");
		EventType::KEY_UP = RegisterEventType("KEY_UP");
		EventType::MOUSE_BUTTON_DOWN = RegisterEventType("MOUSE_BUTTON_DOWN");
		EventType::MOUSE_BUTTON_UP = RegisterEventType("MOUSE_BUTTON_UP");
		EventType::MOUSE_MOTION = RegisterEventType("MOUSE_MOTION");
		EventType::RESIZE = RegisterEventType("RESIZE");
		EventType::SHUTDOWN = RegisterEventType("SHUTDOWN");
	}

	EventManager::~EventManager()
	{

	}

	uint32 EventManager::RegisterEventType(const std::string& name)
	{
		uint32 result = m_Categories.size();
		m_Categories.emplace_back();
		m_Categories[result].name = name;
		m_CategoriesIds[name] = result;
		return result;
	}

	uint32 EventManager::FindEventTypeByName(const std::string& name)
	{
		auto it = m_CategoriesIds.find(name);
		if (it == m_CategoriesIds.end())
		{
			DEBUG_ONLY(
				LOG_WARN("Cannot find event named ", name);
			)
			return 0xffffffff;
		}
		return it->second;
	}

	std::string EventManager::GetEventTypeName(uint32 eventType)
	{
		return m_Categories[eventType].name;
	}

	uint32 EventManager::AddEventListener(uint32 eventType, const EventListener& listener)
	{
		DEBUG_ONLY(
			if (eventType >= m_Categories.size())
			{
				LOG_WARN("Tried to add a listener to an invalid event type ", eventType);
				return 0xffffffff;
			}
		)
		auto& c = m_Categories[eventType];
		uint32 currentIndex = c.currentIndex++;
		c.listeners.push_back(listener);
		c.listeners.back().id = currentIndex;
		for (size_t i = c.listeners.size() - 1; i > 1; i--)
			if (c.listeners[i].priority > c.listeners[i - 1].priority)
				std::swap(c.listeners[i], c.listeners[i - 1]);
			else break;
		return currentIndex;
	}

	void EventManager::RemoveEventListener(uint32 eventType, uint32 id)
	{
		m_Categories[eventType].listeners.erase(
			std::find_if(
				m_Categories[eventType].listeners.begin(),
				m_Categories[eventType].listeners.end(),
				[=](const EventListener& listener) { return listener .id == id; }
			)
		);
	}

	void EventManager::RegisterEvent(Event e)
	{
		DEBUG_ONLY(
			if (e.type >= m_Categories.size())
			{
				LOG_WARN("Tried to register an event with an invalid type ", e.type);
				return;
			}
		)
		for (const auto& listener : m_Categories[e.type].listeners)
			if (listener.function(e))
				break;
	}

	uint32 EventManager::AddEventListener(const std::string& eventTypeName, const EventListener& listener)
	{
		return AddEventListener(FindEventTypeByName(eventTypeName), listener);
	}

	void EventManager::RemoveEventListener(const std::string& eventTypeName, uint32 id)
	{
		RemoveEventListener(FindEventTypeByName(eventTypeName), id);
	}

	void EventManager::RegisterEvent(Event e, const std::string& eventTypeName)
	{
		e.type = FindEventTypeByName(eventTypeName);
		RegisterEvent(e);
	}

}