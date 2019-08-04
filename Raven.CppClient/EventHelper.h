#pragma once
#include "EventHandler.h"

namespace ravendb::client::primitives
{
	class EventHelper
	{
	public:
		~EventHelper() = delete;

		template<class TSender, typename TEvent>
		static void invoke(const std::vector<EventHandler>& delegates, const TSender& sender, const TEvent& event);

		template<typename T>
		static void invoke(const std::vector<std::function<void(const T&)>>& action, const T& argument);
	};

	template <class TSender, typename TEvent>
	void EventHelper::invoke(const std::vector<EventHandler>& delegates, const TSender& sender, const TEvent& event)
	{
		for(const auto& delegate : delegates)
		{
			delegate.handle(&sender, event);
		}
	}

	template <typename T>
	void EventHelper::invoke(const std::vector<std::function<void(const T&)>>& actions,const T& argument)
	{
		for(auto& action : actions)
		{
			action(argument);
		}
	}
}
