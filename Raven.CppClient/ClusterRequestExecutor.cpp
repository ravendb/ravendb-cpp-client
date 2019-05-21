#include "stdafx.h"
#include "ClusterRequestExecutor.h"
#include "GetTcpInfoCommand.h"
#include "GetClusterTopologyCommand.h"

namespace ravendb::client::http
{
	ClusterRequestExecutor::ClusterRequestExecutor(std::optional<impl::CertificateDetails> certificate_details,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		std::shared_ptr<impl::TasksScheduler> scheduler, impl::CurlOptionsSetter set_before_perform,
		impl::CurlOptionsSetter set_after_perform)
		: RequestExecutor({}, std::move(certificate_details), conventions, scheduler,
			set_before_perform, set_after_perform)
	{}

	void ClusterRequestExecutor::perform_health_check(std::shared_ptr<const ServerNode> server_node, int32_t node_index)
	{
		auto command = serverwide::commands::GetTcpInfoCommand("health-check");
		execute(server_node, node_index, command, false, {});
	}

	std::future<void> ClusterRequestExecutor::update_client_configuration_async()
	{
		std::promise<void> promise{};
		promise.set_value();
		return promise.get_future();
	}

	void ClusterRequestExecutor::throw_exceptions(std::string details) const
	{
		std::ostringstream msg{};
		msg << "Failed to retrieve cluster topology from all known nodes" << "\r\n" << std::move(details);
		throw std::runtime_error(msg.str());
	}

	std::shared_ptr<ClusterRequestExecutor> ClusterRequestExecutor::create_for_single_node(const std::string& url,
		std::optional<impl::CertificateDetails> certificate_details,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		std::shared_ptr<impl::TasksScheduler> scheduler, impl::CurlOptionsSetter set_before_perform,
		impl::CurlOptionsSetter set_after_perform)
	{
		auto valid_url = validate_urls({ url }, certificate_details).at(0);

		auto executor = std::shared_ptr<ClusterRequestExecutor>(new ClusterRequestExecutor(std::move(certificate_details),
			conventions ? conventions : documents::conventions::DocumentConventions::default_conventions(),
			scheduler, set_before_perform, set_after_perform));
		executor->_weak_this = executor;

		auto server_node = std::make_shared<ServerNode>();
		server_node->url = std::move(valid_url);

		auto topology = std::make_shared<Topology>();
		topology->etag = -1L;
		topology->nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();
		topology->nodes->emplace_back(server_node);

		executor->_node_selector.emplace(topology, scheduler);
		executor->topology_etag = -2L;
		executor->_disable_client_configuration_updates = true;
		executor->_disable_topology_updates = true;

		return executor;
	}

	std::shared_ptr<ClusterRequestExecutor> ClusterRequestExecutor::create(const std::vector<std::string>& initial_urls,
		std::optional<impl::CertificateDetails> certificate_details, std::shared_ptr<impl::TasksScheduler> scheduler,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		impl::CurlOptionsSetter set_before_perform, impl::CurlOptionsSetter set_after_perform)
	{
		auto executor = std::shared_ptr<ClusterRequestExecutor>(new ClusterRequestExecutor(
			std::move(certificate_details), conventions ? conventions : documents::conventions::DocumentConventions::default_conventions(),
			scheduler, set_before_perform, set_after_perform));
		executor->_weak_this = executor;

		executor->_disable_client_configuration_updates = true;
		executor->_first_topology_update = std::make_shared<std::shared_future<void>>(executor->first_topology_update(initial_urls));
		return executor;
	}

	std::future<bool> ClusterRequestExecutor::update_topology_async(std::shared_ptr<const ServerNode> node, int32_t timeout,
		bool force_update, std::optional<std::string> debug_tag, bool run_async)
	{
		if (_disposed)
		{
			std::promise<bool> promise{};
			promise.set_value(false);
			return promise.get_future();
		}

		auto task = std::make_shared<std::packaged_task<bool()>>(
			[weak_this = _weak_this, node, timeout, force_update, debug_tag = std::move(debug_tag)]()->bool
		{
			auto re = std::static_pointer_cast<ClusterRequestExecutor>(weak_this.lock());
			if (!re)
			{
				return false;
			}
			auto lock = std::unique_lock(re->_cluster_topology_mutex, std::chrono::milliseconds(timeout));
			if (!lock)
			{
				return false;
			}
			try
			{
				if (re->_disposed)
				{
					return false;
				}
				auto command = serverwide::commands::GetClusterTopologyCommand(std::move(debug_tag));
				re->execute(node, {}, command, false, {});

				auto results = command.get_result();

				auto nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();

				for(const auto& [cluster_tag, url] : results->topology.members)
				{
					auto server_node = std::make_shared<ServerNode>();
					server_node->url = url;
					server_node->cluster_tag = cluster_tag;
					nodes->emplace_back(server_node);
				}

				auto new_topology = std::make_shared<Topology>();
				new_topology->nodes = nodes;

				if (!re->_node_selector)
				{
					re->_node_selector.emplace(new_topology, re->_scheduler);

					if (re->get_conventions()->get_read_balance_behavior() == ReadBalanceBehavior::FASTEST_NODE)
					{
						re->_node_selector->schedule_speed_test();
					}
				}
				else if (re->_node_selector->on_update_topology(new_topology, force_update))
				{
					re->dispose_all_failed_nodes_timers();
					if (re->get_conventions()->get_read_balance_behavior() == ReadBalanceBehavior::FASTEST_NODE)
					{
						re->_node_selector->schedule_speed_test();
					}
				}
			}
			catch (...)
			{
				if (!re->_disposed)
				{
					throw;
				}
			}
			return true;
		});

		if (run_async)
		{
			_scheduler->schedule_task_immediately([task]()->void { (*task)(); });
		}
		else
		{
			(*task)();
		}
		return task->get_future();
	}
}
