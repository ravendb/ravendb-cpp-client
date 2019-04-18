#pragma once

namespace ravendb::client::primitives
{
	class EventHelper
	{
	public:
		~EventHelper() = delete;

		//TODO
		//public static <T extends EventArgs> void invoke(List<EventHandler<T>> delegates, Object sender, T event) {
		//	for (EventHandler<T> delegate : delegates) {
		//		delegate.handle(sender, event);
		//	}
		//}

		template<typename T>
		static void invoke(const std::vector<std::function<void(const T&)>>& action, const T& argument);
	};

	template <typename T>
	void EventHelper::invoke(const std::vector<std::function<void(const T&)>>& actions,const T& argument)
	{
		for(auto& action : actions)
		{
			action(argument);
		}
	}
}
