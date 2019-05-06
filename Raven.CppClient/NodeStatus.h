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
		std::weak_ptr<NodeStatus> _weak_this;

		std::chrono::milliseconds _timer_period{};
		const std::weak_ptr<RequestExecutor> _request_executor;
		std::shared_ptr<primitives::Timer> _timer{};

	public:
		const int32_t node_index;
		const std::shared_ptr<const ServerNode> node;

	private:
		std::chrono::milliseconds next_timer_period();

		void timer_callback();

		NodeStatus(std::weak_ptr<RequestExecutor> request_executor, int32_t node_index_param,
			std::shared_ptr<const ServerNode> node_param);

	public:
		static std::shared_ptr<NodeStatus> create (std::weak_ptr<RequestExecutor> request_executor,
			int32_t node_index_param, std::shared_ptr<const ServerNode> node_param);

		void start_timer();

		void update_timer();

		void close();
	};
}
