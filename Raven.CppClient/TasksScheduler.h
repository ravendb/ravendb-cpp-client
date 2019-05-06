#pragma once
#include <thread>
#include <atomic>
#include <condition_variable>
#include "IExecutorService.h"

namespace ravendb::client::impl
{
	class TasksScheduler
	{
	private:
		const std::shared_ptr<IExecutorService> _executor_service;
		std::thread _scheduler{};

		std::multimap<std::chrono::steady_clock::time_point, IExecutorService::Task> _tasks{};

		std::condition_variable _cv{};
		std::mutex _mutex{};

		std::atomic_bool _stop{ false };

		void work()
		{
			while (!_stop)
			{
				IExecutorService::Task task{};
				{
					auto lock = std::unique_lock(_mutex);

					_cv.wait(lock, [this]()->bool {return _stop || !_tasks.empty(); });
					if (_stop)
					{
						return;
					}

					do
					{
						if (_cv.wait_until(lock, _tasks.begin()->first) == std::cv_status::timeout)
						{
							break;
						}
					} while (!_stop);
					if (_stop)
					{
						return;
					}
					_tasks.extract(_tasks.begin()).mapped().swap(task);
				}
				_executor_service->add_task(std::move(task));
			}
		}

	public:
		explicit TasksScheduler(std::shared_ptr<IExecutorService> executor)
			: _executor_service(executor)
			, _scheduler([this]() {work(); })
		{}

		void schedule_task(IExecutorService::Task&& task, std::chrono::milliseconds delay)
		{
			{
				auto lock = std::lock_guard(_mutex);
				_tasks.emplace(std::chrono::steady_clock::now() + delay, std::move(task));
			}
			_cv.notify_one();
		}

		void schedule_task_immediately(IExecutorService::Task&& task)
		{
			_executor_service->add_task(std::move(task));
		}

		~TasksScheduler()
		{
			{
				auto lock = std::lock_guard(_mutex);
				_stop = true;
			}
			_cv.notify_all();

			if(_scheduler.joinable())
			{
				_scheduler.join();
			}
		}
	};
}
