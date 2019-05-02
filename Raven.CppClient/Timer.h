#pragma once
#include <shared_mutex>
#include "TasksScheduler.h"

namespace ravendb::client::primitives
{
	class Timer
	{
	private:
		std::weak_ptr<Timer> _weak_this{};

		const std::shared_ptr<impl::TasksScheduler> _scheduler;
		const impl::IExecutorService::Task _action;
		std::optional<std::chrono::milliseconds> _period{};
		std::shared_mutex _lock{};

		void schedule(std::chrono::milliseconds delay)
		{
			auto task = [timer_ptr = _weak_this]()->void
			{
				if(auto timer = timer_ptr.lock())
				{
					auto period = timer->get_period();

					if (period)
					{
						timer->schedule(*period);
					}
					timer->_action();
				}
			};
			this->_scheduler->schedule_task(std::move(task), delay);
		}

		Timer(std::shared_ptr<impl::TasksScheduler> scheduler,
			impl::IExecutorService::Task&& action,
			std::optional<std::chrono::milliseconds> period = {})
			: _scheduler(scheduler)
			, _action(std::move(action))
			, _period(std::move(period))
		{}

	public:
		static std::shared_ptr<Timer> create(std::shared_ptr<impl::TasksScheduler> scheduler,
			impl::IExecutorService::Task&& action,
			std::chrono::milliseconds delay,
			std::optional<std::chrono::milliseconds> period = {})
		{
			auto object = std::shared_ptr<Timer>(new Timer(scheduler, std::move(action), std::move(period)));
			object->_weak_this = object;

			object->schedule(std::move(delay));

			return object;
		}

		void change(std::optional<std::chrono::milliseconds> period = {})
		{
			auto lock = std::unique_lock(_lock);
			_period = std::move(period);
		}

		std::optional<std::chrono::milliseconds> get_period()
		{
			auto lock = std::shared_lock(_lock);
			return _period;
		}

		void close()
		{
			auto lock = std::unique_lock(_lock);
			_period.reset();
		}
	};
}
