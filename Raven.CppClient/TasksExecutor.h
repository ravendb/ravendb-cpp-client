#pragma once
#include <thread>
#include <queue>
#include "IExecutorService.h"

namespace ravendb::client::impl
{
	class TasksExecutor : public IExecutorService
	{
	public:
		static constexpr size_t DEFAULT_NUMBER_OF_EXECUTION_THREADS = 1;

	private:
		std::vector<std::thread> _pool{};
		std::queue<Task> _tasks{};

		std::condition_variable _cv{};
		std::mutex _mutex{};

		std::atomic_bool _stop_all{ false };

		void work()
		{
			while (!_stop_all)
			{
				Task task{};
				{
					auto lock = std::unique_lock(_mutex);
					_cv.wait(lock, [this]()->bool {return _stop_all || !_tasks.empty(); });

					if(_stop_all)
					{
						return;
					}
					task = _tasks.front();
					_tasks.pop();
				}
				task();
			}
		}

	public:
		explicit TasksExecutor(size_t num_of_threads = DEFAULT_NUMBER_OF_EXECUTION_THREADS)
		{
			_pool.reserve(num_of_threads);
			for (size_t i = 0; i < num_of_threads; ++i)
			{
				_pool.emplace_back([this]() {work(); });
			}
		}

		void add_task(Task task) override
		{
			{
				auto lock = std::lock_guard(_mutex);
				_tasks.push(std::move(task));
			}
			_cv.notify_one();
		}

		~TasksExecutor() override
		{
			{
				auto lock = std::lock_guard(_mutex);
				_stop_all = true;
			}
			_cv.notify_all();

			for(auto& thread : _pool)
			{
				if(thread.joinable())
				{
					thread.join();
				}
			}
		}
	};
}
