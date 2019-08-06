
#include "stdafx.h"
#include "RequestExecutor.h"
#include "GetDatabaseTopologyCommand.h"
#include "GetClientConfigurationOperation.h"
#include <regex>
#include "AuthorizationException.h"
#include "ExceptionDispatcher.h"

namespace ravendb::client::http
{
	documents::operations::GetStatisticsOperation RequestExecutor::failure_check_operation = 
		documents::operations::GetStatisticsOperation("failure=check");

	void RequestExecutor::update_topology_callback()
	{
		auto time = std::chrono::steady_clock::now();
		if(time - _last_returned_response <= std::chrono::minutes(1))
		{
			return;
		}

		auto server_node = std::shared_ptr<const ServerNode>{};

		try
		{
			auto& selector = _node_selector;
			if(!selector)
			{
				return;
			}
			auto preferred_node = selector->get_preferred_node();
			server_node = preferred_node.current_node;
		}
		catch (...)
		{
			//TODO
			//if (logger.isInfoEnabled()) {
			//	logger.info("Couldn't get preferred node Topology from _updateTopologyTimer", e);
			//}
			return;
		}

		try
		{
			update_topology_async(server_node, 0, false, "timer-callback", false).get();
		}
		catch (std::exception& )
		{
			//TODO
			//if (logger.isInfoEnabled()) {
			//	logger.info("Couldn't update topology from _updateTopologyTimer", ex);
			//}
		}
		
	}

	void RequestExecutor::initialize_update_topology_timer()
	{
		if(_update_topology_timer)
		{
			return;
		}

		auto lock = std::lock_guard(_common_mutex);
		if (_update_topology_timer)
		{
			return;
		}
		_update_topology_timer = primitives::Timer::create(_scheduler,
			[weak_this = _weak_this]()->void
		{
			if(auto re = weak_this.lock())
			{
				re->update_topology_callback();
			}

		}, std::chrono::minutes(1), std::chrono::minutes(1)); 
	}

	int64_t RequestExecutor::get_topology_etag() const
	{
		return topology_etag;
	}

	int64_t RequestExecutor::get_client_configuration_etag() const
	{
		return client_configuration_etag;
	}

	RequestExecutor::~RequestExecutor()
	{
		try
		{
			close();
		}
		catch (...)
		{}
	}

	std::shared_future<void> RequestExecutor::first_topology_update(const std::vector<std::string>& input_urls)
	{
		auto initial_urls = validate_urls(input_urls, _certificate_details);

		auto task = std::make_shared<std::packaged_task<void()>>([initial_urls, weak_this = _weak_this]()->void
		{
			auto re = weak_this.lock();
			if(!re)
			{
				return;
			}
			auto list_of_errors = std::unordered_map<std::string, std::string>{};

			for(const auto& url : initial_urls)
			{
				try
				{
					auto server_node = std::make_shared<ServerNode>();
					server_node->url = url;
					server_node->database = re->_database_name.value_or("");

					auto const_server_node = std::const_pointer_cast<ServerNode const>(server_node);

					re->update_topology_async(const_server_node, INT32_MAX, false, "first-topology-update", false).get();

					re->initialize_update_topology_timer();

					re->_topology_taken_from_node = const_server_node;
					return;
				}
				catch (exceptions::security::AuthorizationException&)
				{
					// auth exceptions will always happen, on all nodes
					// so errors immediately
					re->_last_known_urls = std::move(initial_urls);
					throw;
				}
				catch (exceptions::database::DatabaseDoesNotExistException&)
				{
					// Will happen on all node in the cluster,
					// so errors immediately
					re->_last_known_urls = std::move(initial_urls);
					throw;
				}
				catch (std::exception& e)
				{
					if(initial_urls.size() == 1)
					{
						re->_last_known_urls = std::move(initial_urls);
						std::throw_with_nested(std::runtime_error("Cannot get topology from server: " + url));
					}

					list_of_errors.insert_or_assign(url, e.what());
				}
			}

			auto topology = std::make_shared<Topology>();
			topology->etag = re->topology_etag;

			auto topology_nodes = re->get_topology_nodes();
			if(!topology_nodes)
			{
				auto new_topology_nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();
				std::transform(initial_urls.cbegin(), initial_urls.cend(), std::back_inserter(*new_topology_nodes),
					[re](const std::string& url)
				{
					auto server_node = std::make_shared<ServerNode>();
					server_node->url = url;
					server_node->database = re->_database_name.value_or("");
					server_node->cluster_tag = "!";
					return  server_node;
				});
				topology_nodes = std::const_pointer_cast<const std::vector<std::shared_ptr<ServerNode>>>(new_topology_nodes);
			}
			topology->nodes = std::const_pointer_cast<std::vector<std::shared_ptr<ServerNode>>>(topology_nodes);

			re->_node_selector.emplace(topology, re->_scheduler);

			//TODO exists in Java but not in C# -> check
			//if(initial_urls.size() > 0)
			//{
			//	re->initialize_update_topology_timer();
			//	return;
			//}

			re->_last_known_urls = std::move(initial_urls);

			std::ostringstream msg{};
			for(const auto& [url, what] : list_of_errors)
			{
				msg << url << " -> " << what << ", ";
			}
			std::string details = msg.str();
			if(!list_of_errors.empty())
			{
				details.pop_back();
				details.pop_back();
			}
			re->throw_exceptions(std::move(details));
		});

		_scheduler->schedule_task_immediately([task]()->void { (*task)(); });
		return task->get_future().share();
	}

	void RequestExecutor::dispose_all_failed_nodes_timers()
	{
		auto lock = std::lock_guard(_failed_nodes_timers_mutex);

		for(auto& [node, status] : _failed_nodes_timers)
		{
			status->close();
		}
		_failed_nodes_timers.clear();
	}

	void RequestExecutor::throw_exceptions(std::string details) const
	{
		std::ostringstream msg{};
		msg << "Failed to retrieve database topology from all known nodes" << "\r\n" << std::move(details);
		throw std::runtime_error(msg.str());
	}

	void RequestExecutor::configure_curl_before_perform(impl::CurlHandlesHolder::CurlReference& curl_ref) const
	{
		auto curl = curl_ref.get();

		if(_conventions->has_explicitly_set_compression_usage() && !_conventions->is_use_compression())
		{
			curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, nullptr);
		}

		curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);

		if(_certificate_details)//using certificate
		{
			curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, impl::utils::sslctx_function);
			curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, &_certificate_details);
		}

		if (set_before_perform)
		{
			curl_ref.set_before_perform.emplace(set_before_perform);
		}
		if (set_after_perform)
		{
			curl_ref.set_after_perform.emplace(set_after_perform);
		}
	}

	void RequestExecutor::handle_conflict(const impl::CurlResponse& response)
	{
		exceptions::ExceptionDispatcher::throw_exception(response);
	}

	void RequestExecutor::spawn_health_check(std::shared_ptr<const ServerNode> chosen_node, int32_t node_index)
	{
		auto node_status = NodeStatus::create(_weak_this, node_index, chosen_node);

		auto lock = std::lock_guard(_failed_nodes_timers_mutex);

		auto insert_res = _failed_nodes_timers.try_emplace(chosen_node, node_status);

		if(insert_res.second)
		{
			insert_res.first->second->start_timer();
		}
	}

	void RequestExecutor::check_node_status_callback(const NodeStatus& node_status)
	{
		auto copy = get_topology_nodes();

		if(node_status.node_index >= copy->size())
		{
			return; // topology index changed / removed
		}

		auto server_node = std::const_pointer_cast<const ServerNode>(copy->at(node_status.node_index));
		if(!CompareSharedPtrConstServerNode()(server_node, node_status.node))
		{
			return;  // topology changed, nothing to check
		}

		try
		{
			auto status = std::shared_ptr<NodeStatus>();

			try
			{
				perform_health_check(server_node, node_status.node_index);
			}
			catch (std::exception& )
			{
				//TODO
				//if (logger.isInfoEnabled()) {
				//	logger.info(serverNode.getClusterTag() + " is still down", e);
				//{
				auto lock = std::lock_guard(_failed_nodes_timers_mutex);

				if(auto it = _failed_nodes_timers.find(node_status.node);
					it != _failed_nodes_timers.end())
				{
					status = it->second;
					status->update_timer();
				}
				return; // we will wait for the next timer call
			}

			{
				auto lock = std::lock_guard(_failed_nodes_timers_mutex);

				if (auto it = _failed_nodes_timers.find(node_status.node);
					it != _failed_nodes_timers.end())
				{
					status = it->second;
					_failed_nodes_timers.erase(node_status.node);
					status->close();
				}
			}
			if(_node_selector)
			{
				_node_selector->restore_node_index(node_status.node_index);
			}
		}
		catch (std::exception& )
		{
			//TODO
			// if (logger.isInfoEnabled()) {
			//	logger.info("Failed to check node topology, will ignore this node until next topology update", e);
			//}
		}
	}

	void RequestExecutor::ensure_node_selector()
	{
		if(_first_topology_update)
		{
			_first_topology_update->get();
		}

		if(!_node_selector)
		{
			auto topolody = std::make_shared<Topology>();
			topolody->nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();

			const auto& topology_nodes = *get_topology_nodes();

			topolody->nodes->reserve(topology_nodes.size());
			for (const auto& server_node : topology_nodes)
			{
				topolody->nodes->emplace_back(std::make_shared<ServerNode>(*server_node));
			}
			_node_selector.emplace(topolody, _scheduler);
		}
	}

	RequestExecutor::RequestExecutor(
		std::optional<std::string> database_name,
		std::optional<impl::CertificateDetails> certificate_details,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		std::shared_ptr<impl::TasksScheduler> scheduler,
		impl::CurlOptionsSetter set_before_perform,
		impl::CurlOptionsSetter set_after_perform)
		: _curl_holder()
		, _certificate_details(std::move(certificate_details))
		, _database_name(std::move(database_name))
		, _last_returned_response(std::chrono::steady_clock::now())
		, _cache(HttpCache::create(conventions->get_max_http_cache_size()))
		, _conventions(documents::conventions::DocumentConventions::clone(conventions))
		, _scheduler(scheduler)
		, set_before_perform(set_before_perform)
		, set_after_perform(set_after_perform)
	{
		//, _initial_urls(std::move(initial_urls))
//	, _topology(std::make_shared<Topology>())
//{
//	_topology->nodes.reserve(_initial_urls.size());

//	for (const auto& url : _initial_urls)
//	{
//		_topology->nodes.emplace_back(url, _database_name, "?");
//	}
//	_topology->etag = -1;
//}
	}

	std::future<void> RequestExecutor::update_client_configuration_async()
	{
		if (_disposed)
		{
			std::promise<void> promise{};
			promise.set_value();
			return promise.get_future();
		}

		auto task = std::make_shared<std::packaged_task<void()>>(
			[weak_this = _weak_this]()->void
		{
			auto re = weak_this.lock();
			if (!re)
			{
				return;
			}

			auto lock = std::unique_lock<std::mutex>(re->_update_client_configuration_mutex);

			auto old_disable_client_configuration_updates = re->_disable_client_configuration_updates;
			re->_disable_client_configuration_updates = true;
			try
			{
				if (re->_disposed)
				{
					return;
				}

				auto command = documents::operations::configuration::GetClientConfigurationOperation().get_command(re->_conventions);
				auto current_index_and_node = re->choose_node_for_request(*command, {});
				re->execute(current_index_and_node.current_node, current_index_and_node.current_index, *command, false, {});

				if(auto result = command->get_result())
				{
					re->_conventions->update_from(result->configuration);
					re->client_configuration_etag = result->etag;
				}
			}
			catch (...)
			{
				re->_disable_client_configuration_updates = old_disable_client_configuration_updates;
				throw;
			}
			re->_disable_client_configuration_updates = old_disable_client_configuration_updates;
		});

		_scheduler->schedule_task_immediately([task]()->void { (*task)(); });
		return task->get_future();
	}

	std::vector<std::string> RequestExecutor::validate_urls(const std::vector<std::string>& initial_urls,
		const std::optional<impl::CertificateDetails>& certificate)
	{
		auto clean_urls = std::vector<std::string>{};
		clean_urls.reserve(initial_urls.size());
		const bool require_https = certificate.has_value();

		//checking urls according to :
		//https://tools.ietf.org/html/rfc3986#page-50

		for (const auto& url : initial_urls)
		{
			std::regex url_regex(
				R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)",
				std::regex::extended);
			std::smatch url_match_result;

			if (std::regex_match(url, url_match_result, url_regex))
			{
				auto scheme = url_match_result[2];
				if (require_https)
				{
					if (scheme != "https")// not a match
					{
						std::ostringstream msg{};
						msg << "Unable to use url '" << url << "', a certificate was specified, so 'https' must be used as the url scheme";
						throw std::invalid_argument(msg.str());
					}
				}
				else if (scheme != "http")// not a match
				{
					std::ostringstream msg{};
					msg << "Unable to use url '" << url << "', a certificate was NOT specified, so 'http' must be used as the url scheme";
					throw std::invalid_argument(msg.str());
				}
			}
			else
			{
				std::ostringstream msg{};
				msg << "The url: '" << url << "' is malformed";
				throw std::invalid_argument(msg.str());
			}

			clean_urls.emplace_back(url.begin(), (url.back() == '/') ? --url.end() : url.end());
		}
		return std::move(clean_urls);
	}

	void RequestExecutor::perform_health_check(std::shared_ptr<const ServerNode> server_node, int32_t node_index)
	{
		auto command = failure_check_operation.get_command(_conventions);
		execute(server_node, node_index, *command, false, {});
	}

	std::shared_ptr<const Topology> RequestExecutor::get_topology() const
	{
		return _node_selector ? _node_selector->get_topology() : std::shared_ptr<const Topology>();
	
	}

	std::shared_ptr<const std::vector<std::shared_ptr<ServerNode>>> RequestExecutor::get_topology_nodes() const
	{
		if(auto topology = get_topology())
		{
			return std::const_pointer_cast<const std::vector<std::shared_ptr<ServerNode>>>(topology->nodes);
		}
		return {};
	}

	std::optional<std::string> RequestExecutor::get_url() const
	{
		if(!_node_selector)
		{
			return {};
		}

		return  _node_selector->get_preferred_node().current_node->url;
	}

	std::shared_ptr<documents::conventions::DocumentConventions> RequestExecutor::get_conventions() const
	{
		return _conventions;
	}

	const std::optional<impl::CertificateDetails>& RequestExecutor::get_certificate() const
	{
		return _certificate_details;
	}

	std::future<bool> RequestExecutor::update_topology_async(std::shared_ptr<const ServerNode> node, int32_t timeout,
		bool force_update, std::optional<std::string> debug_tag, bool run_async)
	{
		if(_disposed)
		{
			std::promise<bool> promise{};
			auto fut = promise.get_future();
			promise.set_value(false);
			return std::move(fut);
		}

		auto task = std::make_shared<std::packaged_task<bool()>>(
			[weak_this = _weak_this, node, timeout, force_update, debug_tag = std::move(debug_tag)]()->bool
		{
			auto re = weak_this.lock();
			if(!re)
			{
				return false;
			}
			auto lock = std::unique_lock(re->_update_database_topology_mutex, std::chrono::milliseconds(timeout));
			if(!lock)
			{
				return false;
			}
			try
			{
				if(re->_disposed)
				{
					return false;
				}
				auto command = serverwide::commands::GetDatabaseTopologyCommand(std::move(debug_tag));
				re->execute(node, {}, command, false, {});

				if(!re->_node_selector)
				{
					re->_node_selector.emplace(command.get_result(), re->_scheduler);

					if(re->get_conventions()->get_read_balance_behavior() == ReadBalanceBehavior::FASTEST_NODE)
					{
						re->_node_selector->schedule_speed_test();
					}
				}
				else if(re->_node_selector->on_update_topology(command.get_result(), force_update))
				{
					re->dispose_all_failed_nodes_timers();
					if (re->get_conventions()->get_read_balance_behavior() == ReadBalanceBehavior::FASTEST_NODE)
					{
						re->_node_selector->schedule_speed_test();
					}
				}

				re->topology_etag = re->_node_selector->get_topology()->etag.value_or(-1);
			}
			catch (...)
			{
				if(!re->_disposed)
				{
					throw;
				}
			}
			return true;
		});

		if(run_async)
		{
			_scheduler->schedule_task_immediately([task]()->void { (*task)(); });
		}
		else
		{
			(*task)();
		}
		return task->get_future();
	}

	bool RequestExecutor::in_speed_test_phase() const
	{
		return _node_selector ? _node_selector->in_speed_test_phase() : false;
	}

	std::shared_ptr<const ServerNode> RequestExecutor::handle_server_not_responsive(const std::string& url,
		std::shared_ptr<const ServerNode> chosen_node, int32_t node_index, std::exception_ptr e)
	{
		spawn_health_check(chosen_node, node_index);
		if(_node_selector)
		{
			_node_selector->on_failed_request(node_index);
		}

		auto preferred_node = get_preferred_node();
		try
		{
			update_topology_async(preferred_node.current_node, 0, true, "handle-server-not-responsive", false).get();
		}
		catch (...)
		{
			std::rethrow_exception(e);
		}

		return preferred_node.current_node;
	}

	void RequestExecutor::close()
	{
		if(_disposed)
		{
			return;
		}
		_disposed = true;
		//cache.close();

		if(_update_topology_timer)
		{
			_update_topology_timer->close();
		}
		dispose_all_failed_nodes_timers();
	}

	CurrentIndexAndNode RequestExecutor::get_preferred_node()
	{
		ensure_node_selector();
		return _node_selector->get_preferred_node();
	}

	CurrentIndexAndNode RequestExecutor::get_node_by_session_id(int32_t session_id)
	{
		ensure_node_selector();
		return _node_selector->get_node_by_session_id(session_id);
	}

	CurrentIndexAndNode RequestExecutor::get_fastest_node()
	{
		ensure_node_selector();
		return _node_selector->get_fastest_node();
	}

	std::shared_ptr<RequestExecutor> RequestExecutor::create(
		const std::vector<std::string>& initial_urls,
		std::optional<std::string> database_name,
		std::optional<impl::CertificateDetails> certificate_details,
		std::shared_ptr<impl::TasksScheduler> scheduler,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		impl::CurlOptionsSetter set_before_perform,
		impl::CurlOptionsSetter set_after_perform)
	{
		auto executor = std::shared_ptr<RequestExecutor>(new RequestExecutor(
			std::move(database_name),
			std::move(certificate_details),
			conventions,
			scheduler,
			set_before_perform,
			set_after_perform));

		executor->_weak_this = executor;
		executor->_first_topology_update = std::make_shared<std::shared_future<void>>(executor->first_topology_update(initial_urls));

		return executor;
	}

	std::shared_ptr<RequestExecutor> RequestExecutor::create_for_single_node_with_configuration_updates(std::string url,
		std::optional<std::string> database_name, std::optional<impl::CertificateDetails> certificate_details,
		std::shared_ptr<impl::TasksScheduler> scheduler,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		impl::CurlOptionsSetter set_before_perform, impl::CurlOptionsSetter set_after_perform)
	{
		auto executor = create_for_single_node_without_configuration_updates(std::move(url),
			std::move(database_name), std::move(certificate_details), scheduler, conventions,
			set_before_perform, set_after_perform);

		executor->_disable_client_configuration_updates = false;
		return executor;
	}

	std::shared_ptr<RequestExecutor> RequestExecutor::create_for_single_node_without_configuration_updates(
		std::string url, std::optional<std::string> database_name,
		std::optional<impl::CertificateDetails> certificate_details, std::shared_ptr<impl::TasksScheduler> scheduler,
		std::shared_ptr<documents::conventions::DocumentConventions> conventions,
		impl::CurlOptionsSetter set_before_perform, impl::CurlOptionsSetter set_after_perform)
	{
		const auto initial_urls = validate_urls({ std::move(url) }, certificate_details);

		auto executor = std::shared_ptr<RequestExecutor>(new RequestExecutor(
			std::move(database_name),
			std::move(certificate_details),
			conventions,
			scheduler,
			set_before_perform,
			set_after_perform));

		executor->_weak_this = executor;

		auto topology = std::make_shared<Topology>();
		topology->etag.emplace(-1);

		auto server_node = std::make_shared<ServerNode>();
		server_node->database = executor->_database_name.value_or("");
		server_node->url = initial_urls.at(0);
		topology->nodes = std::make_shared<std::vector<std::shared_ptr<ServerNode>>>();
		topology->nodes->emplace_back(server_node);

		executor->_node_selector.emplace(topology, executor->_scheduler);
		executor->topology_etag = -2;
		executor->_disable_topology_updates = true;
		executor->_disable_client_configuration_updates = true;

		return executor;
	}

	std::shared_ptr<HttpCache> RequestExecutor::get_cache() const
	{
		return _cache;
	}
}
