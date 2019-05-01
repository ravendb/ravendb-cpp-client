#pragma once

namespace ravendb::client::impl
{
	class IExecutorService
	{
	public:
		using Task = std::function<void()>;

		virtual ~IExecutorService() = 0;

		virtual void add_task(Task task) = 0;
	};

	inline IExecutorService::~IExecutorService() = default;
}
