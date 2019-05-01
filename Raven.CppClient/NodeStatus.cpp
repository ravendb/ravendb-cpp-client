#include "stdafx.h"
#include "NodeStatus.h"
#include "RequestExecutor.h"
#include "ServerNode.h"

namespace ravendb::client::http
{
	std::chrono::milliseconds NodeStatus::next_timer_period()
	{
		static const auto PERIOD = std::chrono::seconds(5);

		if (_timer_period >= PERIOD)
		{
			return PERIOD;
		}
		return _timer_period = _timer_period + std::chrono::milliseconds(100);
	}

	void NodeStatus::start_timer(std::chrono::milliseconds next_timer_period)
	{
		auto re = _request_executor.lock();
		
		_timer = primitives::Timer::create(re->_scheduler, [this]() {timer_callback(); }, _timer_period);
		
	}

	void NodeStatus::update_timer()
	{
		_timer->change(next_timer_period());
	}

	void NodeStatus::timer_callback()
	{
		if(auto re = _request_executor.lock())
		{
			if(!re->_disposed)
			{
				re->check_node_status_callback(*this);
			}
		}
	}

	NodeStatus::NodeStatus(std::weak_ptr<RequestExecutor> request_executor, int32_t node_index_param,
		std::shared_ptr<ServerNode> node_param)
		: _timer_period(std::chrono::milliseconds(100))
		, _request_executor(request_executor)
		, node_index(node_index_param)
		, node(node_param)
	{}

	void NodeStatus::close()
	{
		_timer->close();
	}
}
