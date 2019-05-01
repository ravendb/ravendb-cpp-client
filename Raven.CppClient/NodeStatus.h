#pragma once
#include "Timer.h"

namespace ravendb::client
{
	namespace http
	{
		class RequestExecutor;
		struct ServerNode;
	}
	namespace primitives
	{
		class Timer;
	}
}

namespace ravendb::client::http
{
	class NodeStatus
	{
	private:
		std::chrono::milliseconds _timer_period{};
		const std::weak_ptr<RequestExecutor> _request_executor;
		std::shared_ptr<primitives::Timer> _timer{};

	public:
		const int32_t node_index;
		const std::shared_ptr<ServerNode> node;

	private:
		std::chrono::milliseconds next_timer_period();

		void start_timer(std::chrono::milliseconds next_timer_period);

		void update_timer();

		void timer_callback();

	public:
		NodeStatus(std::weak_ptr<RequestExecutor> request_executor, int32_t node_index_param, std::shared_ptr<ServerNode> node_param);

		void close();
	};
}
