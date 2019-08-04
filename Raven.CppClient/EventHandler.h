#pragma once
#include <typeindex>
#include <functional>
#include "EventArgs.h"

namespace ravendb::client::primitives
{
	class EventHandler
	{
	private:
		//using std::shared_ptr for operator== and copy ctor
		std::shared_ptr<std::function<void(const void* sender, const EventArgs& event)>> _handler;

	public:
		template<typename TSender, typename TEvent>
		EventHandler(std::function<void(const TSender* sender, const TEvent& event)> handler);

		template<typename TSender, typename TEvent>
		void handle(const TSender* sender, const TEvent& event) const;

		template<typename TSender, typename TEvent>
		void operator()(const TSender* sender, const TEvent& event) const;


		friend bool operator==(const EventHandler& lhs, const EventHandler& rhs)
		{
			return lhs._handler == rhs._handler;
		}

		friend bool operator!=(const EventHandler& lhs, const EventHandler& rhs)
		{
			return !(lhs == rhs);
		}
	};

	template <typename TSender, typename TEvent>
	EventHandler::EventHandler(std::function<void(const TSender* sender, const TEvent& event)> handler)
		: _handler(std::make_shared<std::function<void(const void* sender, const EventArgs& event)>>(
			[handler = std::move(handler)](const void* sender, const EventArgs& event)-> void
			{
				handler(static_cast<const TSender*>(sender), static_cast<const TEvent&>(event));
			}))
		{
			static_assert(std::is_base_of_v<EventArgs, TEvent>, "TEvent must be derived from EventArgs");
		}

	template <typename TSender, typename TEvent>
	void EventHandler::handle(const TSender* sender, const TEvent& event) const
	{
		this->operator()(sender, event);
	}

	template <typename TSender, typename TEvent>
	void EventHandler::operator()(const TSender* sender, const TEvent& event) const
	{
		static_assert(std::is_base_of_v<EventArgs, TEvent>, "TEvent must be derived from EventArgs");

		(*_handler)(static_cast<const void*>(&sender), static_cast<const EventArgs&>(event));
	}
}
