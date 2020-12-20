#pragma once

#include "event.h"
#include "eventListener.h"

namespace fmega {

	struct EventCategory
	{
		uint32 currentIndex = 0;
		std::string name;
		std::vector<EventListener> listeners;
	};

	class EventManager
	{
	public:
		EventManager();
		~EventManager();
		uint32 RegisterEventType(const std::string& name);
		uint32 FindEventTypeByName(const std::string& name);
		std::string GetEventTypeName(uint32 eventType);
		uint32 AddEventListener(const std::string& eventTypeName, const EventListener& listener);
		uint32 AddEventListener(uint32 eventType, const EventListener& listener);
		void RemoveEventListener(const std::string& eventTypeName, uint32 id);
		void RemoveEventListener(uint32 eventType, uint32 id);
		void RegisterEvent(Event e);
		void RegisterEvent(Event e, const std::string& eventTypeName);
	private:
		std::vector<EventCategory> m_Categories;
		std::unordered_map<std::string, uint32> m_CategoriesIds;
	};

}
