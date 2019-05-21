#pragma once
#include <future>
#include "CurlHandlesHolder.h"
#include "Topology.h"
#include "RavenCommand.h"
#include "utils.h"
#include "CertificateDetails.h"
#include "DocumentConventions.h"
#include "GetStatisticsOperation.h"
#include "NodeStatus.h"
#include "TasksScheduler.h"
#include "HttpCache.h"
#include "NodeSelector.h"
#include "SessionInfo.h"
#include "CurlResponse.h"
#include "HttpExtensions.h"
#include "ResponseDisposeHandling.h"
#include "GetCppClassName.h"
#include "HttpStatus.h"
#include "DatabaseDoesNotExistException.h"
#include "AuthorizationException.h"
#include "ClientVersionMismatchException.h"
#include "AllTopologyNodesDownException.h"
#include "UnsuccessfulRequestException.h"
#include "ExceptionDispatcher.h"
#include "CurlException.h"

namespace ravendb::client::http
{
	class RequestExecutor
	{
	public:
		friend class NodeStatus;

		using IndexAndResponse = std::pair<int32_t, impl::CurlResponse>;

	private:
		std::mutex _common_mutex{};

		static documents::operations::GetStatisticsOperation failure_check_operation;

		impl::CurlHandlesHolder _curl_holder{};

		//TODO update
		static constexpr char CLIENT_VERSION[] = "4.1.0";

		std::timed_mutex _update_database_topology_mutex{};
		std::mutex _update_client_configuration_mutex{};

		std::unordered_map<std::shared_ptr<const ServerNode>, std::shared_ptr<NodeStatus>,
			std::hash<std::shared_ptr<const ServerNode>>, CompareSharedPtrConstServerNode> _failed_nodes_timers;
		std::shared_mutex _failed_nodes_timers_mutex{};

		const std::optional<impl::CertificateDetails> _certificate_details;

		const std::optional<std::string> _database_name;

		//TODO private static final Log logger = LogFactory.getLog(RequestExecutor.class);

		std::chrono::steady_clock::time_point _last_returned_response;

		HttpCache cache{};//TODO implement it!

		std::shared_ptr<const ServerNode> _topology_taken_from_node{};

		//TODO think of an alternative
		//public final ThreadLocal<AggressiveCacheOptions> aggressiveCaching = new ThreadLocal<>();

		std::shared_ptr<primitives::Timer> _update_topology_timer{};

		const std::shared_ptr<documents::conventions::DocumentConventions> _conventions;



		//std::vector<std::string> _initial_urls;
		//std::shared_ptr<Topology> _topology;

	protected:
		std::weak_ptr<RequestExecutor> _weak_this{};

		const std::shared_ptr<impl::TasksScheduler> _scheduler;

		std::optional<NodeSelector> _node_selector{};

		int64_t topology_etag{};
		int64_t client_configuration_etag{};

		bool _disable_topology_updates{ false };
		bool _disable_client_configuration_updates{ false };

		bool _disposed{ false };

		std::shared_ptr<std::shared_future<void>> _first_topology_update;

		std::vector<std::string> _last_known_urls{};



	public:
		//Extension point to plug - in request pre/post processing like adding proxy etc.
		impl::CurlOptionsSetter set_before_perform = {};
		impl::CurlOptionsSetter set_after_perform = {};

		std::atomic_int64_t number_of_server_requests{ 0 };





	private:
		void initialize_update_topology_timer();

		template<typename TResult>
		void unlikely_execute(RavenCommand<TResult>& command, std::shared_ptr<std::shared_future<void>>& topology_update,
			const std::optional<documents::session::SessionInfo>& session_info);

		void update_topology_callback();



		//TODO old version for checks
		template<typename TResult>
		std::shared_ptr<TResult> execute_internal(ServerNode& node, RavenCommand<TResult>& command);

		template<typename TResult>
		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, 
			std::shared_ptr<const ServerNode> node, RavenCommand<TResult>& command, std::optional<std::string>& url) const;

		void configure_curl_before_perform(impl::CurlHandlesHolder::CurlReference& curl_ref) const;

		template<typename TResult>
		void throw_failed_to_contact_all_nodes(const RavenCommand<TResult>& command,
			const impl::CurlHandlesHolder::CurlReference& curl_ref, std::exception_ptr e,
			std::exception_ptr timeout_exception) const;

		template<typename TResult>
		bool should_execute_on_all(std::shared_ptr<const ServerNode>, const RavenCommand<TResult>& command) const;

		template<typename TResult>
		impl::CurlResponse execute_on_all_to_figure_out_the_fastest(std::shared_ptr<const ServerNode> chosen_node,
			RavenCommand<TResult>& command);

		//TODO
		//private <TResult> HttpCache.ReleaseCacheItem getFromCache(RavenCommand<TResult> command, boolean useCache, String url, Reference<String> cachedChangeVector, Reference<String> cachedValue)
		
		template<typename TResult>
		bool handle_unsuccessful_response(std::shared_ptr<const ServerNode> chosen_node,
			const std::optional<int32_t>& node_index, RavenCommand<TResult>& command,
			const impl::CurlHandlesHolder::CurlReference& curl_ref, const impl::CurlResponse& response,
			const std::string& url, const std::optional<documents::session::SessionInfo>& session_info,
			bool should_retry);

		static void handle_conflict(const impl::CurlResponse& response);

		template<typename TResult>
		bool handle_server_down(std::shared_ptr<const ServerNode> chosen_node,
			const std::optional<int32_t>& node_index, RavenCommand<TResult>& command,
			const impl::CurlHandlesHolder::CurlReference& curl_ref, const impl::CurlResponse& response,
			std::exception_ptr e, const std::optional<documents::session::SessionInfo>& session_info);

		void spawn_health_check(std::shared_ptr<const ServerNode> chosen_node, int32_t node_index);

		void check_node_status_callback(const NodeStatus& node_status);

		template<typename TResult>
		static void add_failed_response_to_command(std::shared_ptr<const ServerNode> chosen_node,
			RavenCommand<TResult>& command, const impl::CurlHandlesHolder::CurlReference& curl_ref,
			const impl::CurlResponse& response, std::exception_ptr e);

		void ensure_node_selector();

	protected:
		RequestExecutor(
			std::optional<std::string> database_name,
			std::optional<impl::CertificateDetails> certificate_details,			
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		virtual std::future<void> update_client_configuration_async();

		void dispose_all_failed_nodes_timers();

		std::shared_future<void> first_topology_update(const std::vector<std::string>& input_urls);

		virtual void throw_exceptions(std::string details) const;

		static std::vector<std::string> validate_urls(const std::vector<std::string>& initial_urls,
			const std::optional<impl::CertificateDetails>& certificate);

		virtual void perform_health_check(std::shared_ptr<const ServerNode> server_node, int32_t node_index);


	public:
		virtual ~RequestExecutor();

		RequestExecutor(const RequestExecutor& other) = delete;
		RequestExecutor(RequestExecutor&& other) = delete;
		RequestExecutor& operator=(const RequestExecutor& other) = delete;
		RequestExecutor& operator=(RequestExecutor&& other) = delete;

		static std::shared_ptr<RequestExecutor> create(
			const std::vector<std::string>& initial_urls,
			std::optional<std::string> database_name,
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		static std::shared_ptr<RequestExecutor> create_for_single_node_with_configuration_updates(
			std::string url,
			std::optional<std::string> database_name,
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		static std::shared_ptr<RequestExecutor> create_for_single_node_without_configuration_updates(
			std::string url,
			std::optional<std::string> database_name,
			std::optional<impl::CertificateDetails> certificate_details,
			std::shared_ptr<impl::TasksScheduler> scheduler,
			std::shared_ptr<documents::conventions::DocumentConventions> conventions,
			impl::CurlOptionsSetter set_before_perform = {},
			impl::CurlOptionsSetter set_after_perform = {});

		std::shared_ptr<const Topology> get_topology() const;

		std::shared_ptr<const std::vector<std::shared_ptr<ServerNode>>> get_topology_nodes() const;

		std::optional<std::string> get_url() const;

		int64_t get_topology_etag() const;

		int64_t get_client_configuration_etag() const;

		std::shared_ptr<documents::conventions::DocumentConventions> get_conventions() const;

		const std::optional<impl::CertificateDetails>& get_certificate() const;

		virtual std::future<bool> update_topology_async(std::shared_ptr<const ServerNode> node, int32_t timeout,
			bool force_update = false, std::optional<std::string> debug_tag = {}, bool run_async = true);

		template<typename TResult>
		void execute(RavenCommand<TResult>& command, 
			const std::optional<documents::session::SessionInfo>& session_info = {});

		template<typename TResult>
		CurrentIndexAndNode choose_node_for_request(RavenCommand<TResult>& cmd,
			const std::optional<documents::session::SessionInfo>& session_info);

		template<typename TResult>
		void execute(
			std::shared_ptr<const ServerNode> chosen_node,
			const std::optional<int32_t>& node_index,
			RavenCommand<TResult>& command,
			bool should_retry,
			const std::optional<documents::session::SessionInfo>& session_info);

		bool in_speed_test_phase() const;

		std::shared_ptr<const ServerNode> handle_server_not_responsive(const std::string& url,
			std::shared_ptr<const ServerNode> chosen_node, int32_t node_index, std::exception_ptr e);

		void close();

		CurrentIndexAndNode get_preferred_node();

		CurrentIndexAndNode get_node_by_session_id(int32_t session_id);

		CurrentIndexAndNode get_fastest_node();
	};

	template <typename TResult>
	void RequestExecutor::unlikely_execute(RavenCommand<TResult>& command,
		std::shared_ptr<std::shared_future<void>>& topology_update,
		const std::optional<documents::session::SessionInfo>& session_info)
	{
		try
		{
			if(!topology_update)
			{
				auto lock = std::unique_lock(_common_mutex);
				if(!_first_topology_update)
				{
					if(_last_known_urls.empty())
					{
						throw std::runtime_error("No known topology and no previously known one, cannot proceed, likely a bug");
					}
					_first_topology_update = std::make_shared<std::shared_future<void>>(first_topology_update(_last_known_urls));
				}
				topology_update = _first_topology_update;
			}

			topology_update->get();
		}
		catch (...)
		{
			{
				auto lock = std::unique_lock(_common_mutex);
				if (_first_topology_update == topology_update)
				{
					_first_topology_update.reset();
				}
			}
			throw;
		}

		auto current_index_and_node = choose_node_for_request(command, session_info);
		execute(current_index_and_node.current_node, current_index_and_node.current_index, command, true, session_info);
	}

	//TODO old code -> remove later
	//template<typename TResult>
	//std::shared_ptr<TResult> RequestExecutor::execute_internal(ServerNode & node, RavenCommand<TResult>& command)
	//{
	//	auto curl_ref = _curl_holder.checkout_curl();
	//	auto curl = curl_ref.get();

	//	std::string url;
	//	command.create_request(curl_ref, node, url);
	//	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	//	if (_certificate_details)//using certificate
	//	{
	//		curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, ravendb::client::impl::utils::sslctx_function);
	//		curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, &_certificate_details);
	//	}

	//	if (set_before_perform)
	//	{
	//		set_before_perform(curl);
	//	}

	//	const auto response = impl::CurlResponse::run_curl_perform(curl_ref);

	//	if (response.perform_result != CURLE_OK)
	//	{
	//		std::ostringstream error_builder;
	//		error_builder << "Failed request to: "
	//			<< url
	//			<< ", status code: "
	//			<< std::to_string(response.status_code)
	//			<< "\n"
	//			<< response.error
	//			<< "\n";

	//		throw RavenError(error_builder.str(), RavenError::ErrorType::GENERIC);
	//	}
	//	if (set_after_perform)
	//	{
	//		set_after_perform(curl);
	//	}

	//	command.status_code = response.status_code;
	//	switch (response.status_code)
	//	{
	//	case 200:
	//	case 201:
	//	{
	//		auto result = response.output.empty() ? nlohmann::json() : nlohmann::json::parse(response.output);
	//		command.set_response(curl, result, false);
	//		break;
	//	}
	//	case 204:
	//		break;
	//	case 304:
	//		break;
	//	case 404:
	//		command.set_response_not_found(curl);
	//		break;
	//	case 503:
	//		if (response.headers.find("Database-Missing") != response.headers.end())
	//		{
	//			throw RavenError(response.output, RavenError::ErrorType::DATABASE_DOES_NOT_EXIST);
	//		}
	//		throw RavenError(response.output, RavenError::ErrorType::SERVICE_NOT_AVAILABLE);
	//	case 500:
	//		throw RavenError(response.output, RavenError::ErrorType::INTERNAL_SERVER_ERROR);
	//	default:
	//		throw RavenError(response.output, RavenError::ErrorType::UNEXPECTED_RESPONSE);
	//	}

	//	return command.get_result();
	//}

	template <typename TResult>
	void RequestExecutor::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, 
		std::shared_ptr<const ServerNode> node, RavenCommand<TResult>& command, std::optional<std::string>& url) const
	{
		command.create_request(curl_ref, node, url);
		curl_ref.url = *url;

		curl_ref.headers.try_emplace(constants::headers::CLIENT_VERSION, CLIENT_VERSION);

		configure_curl_before_perform(curl_ref);
	}

	template <typename TResult>
	void RequestExecutor::throw_failed_to_contact_all_nodes(const RavenCommand<TResult>& command,
		const impl::CurlHandlesHolder::CurlReference& curl_ref, std::exception_ptr e,
		std::exception_ptr timeout_exception) const
	{
		std::ostringstream msg{};
		msg <<
			"We tried to send " << command.result_type_name <<
			" request via " << curl_ref.method << " " << curl_ref.url <<
			" to all configured nodes in the topology, " <<
			"all of them seem to be down or not responding. We've tried to access the following nodes: ";

		if(_node_selector)
		{
			for(const auto& node : *_node_selector->get_topology()->nodes)
			{
				msg << node->url << ", ";
			}
		}

		if(_topology_taken_from_node)
		{
			std::ostringstream nodes{};
			if (_node_selector)
			{
				for (const auto& node : *_node_selector->get_topology()->nodes)
				{
					nodes << "( url: " << node->url <<
						", clusterTag: " << node->cluster_tag <<
						", serverRole: " << nlohmann::json(node->server_role).get<std::string>() << ")" << ", ";
				}
			}

			msg << "\n\r" << "We was able to fetch " << _topology_taken_from_node->database <<
				" topology from " << _topology_taken_from_node->url << ".\n\r" <<
				"Fetched topology: " << nodes.rdbuf();
		}
		try
		{
			std::rethrow_exception(timeout_exception ? timeout_exception : e);
		}
		catch (...)
		{
			std::throw_with_nested(exceptions::AllTopologyNodesDownException(msg.str()));
		}
	}

	template <typename TResult>
	bool RequestExecutor::should_execute_on_all(std::shared_ptr<const ServerNode> chosen_node,
		const RavenCommand<TResult>& command) const
	{
		return get_conventions()->get_read_balance_behavior() == ReadBalanceBehavior::FASTEST_NODE &&
			_node_selector && _node_selector->in_speed_test_phase() &&
			_node_selector->get_topology()->nodes->size() > 1 &&
			command.is_read_request() &&
			command.get_response_type() == RavenCommandResponseType::OBJECT &&
			chosen_node;
	}

	template <typename TResult>
	impl::CurlResponse RequestExecutor::execute_on_all_to_figure_out_the_fastest(
		std::shared_ptr<const ServerNode> chosen_node, RavenCommand<TResult>& command)
	{
		std::atomic_int32_t number_of_failed_tasks{ 0 };
		std::shared_ptr<std::shared_future<IndexAndResponse>> preferred_task{};

		const auto& nodes = *_node_selector->get_topology()->nodes;

		auto tasks = std::vector<std::shared_ptr<std::shared_future<IndexAndResponse>>>(nodes.size());

		for(int32_t i = 0; i < nodes.size(); ++i)
		{
			++number_of_server_requests;

			auto pack_task = std::make_shared<std::packaged_task<IndexAndResponse()>>(
				[&, task_number = i]()->IndexAndResponse
			{
				try
				{
					auto url_ref = std::optional<std::string>();
					auto curl_ref = _curl_holder.checkout_curl();
					create_request(curl_ref, nodes[task_number], command, url_ref);
					return IndexAndResponse(task_number, command.send(curl_ref));
				}
				catch (...)
				{
					++number_of_failed_tasks;
					tasks[task_number].reset();
					std::throw_with_nested(std::runtime_error("Request execution failed"));
				}
			});

			_scheduler->schedule_task_immediately([pack_task]()->void {(*pack_task)(); });
			tasks[i] = std::make_shared<std::shared_future<IndexAndResponse>>(pack_task->get_future());
			
			if(nodes[i]->cluster_tag == chosen_node->cluster_tag)
			{
				preferred_task = tasks[i];
			}
		}

		while(number_of_failed_tasks < tasks.size())
		{
			int32_t i = 0;
			for(; i < tasks.size(); ++i)
			{
				if(!tasks[i])
				{
					continue;
				}
				auto res = tasks[i]->wait_for(std::chrono::milliseconds(0));
				switch (res)
				{
				case std::future_status::deferred:
				case std::future_status::timeout:
					continue;
				case std::future_status::ready:
					;
				}
				try
				{
					const auto fastest = tasks[i]->get();
					_node_selector->record_fastest(fastest.first, nodes[fastest.first]);
					break;
				}
				catch (...)
				{
					++number_of_failed_tasks;
					tasks[i].reset();
				}
			}
			if(i < tasks.size())
			{
				break;
			}
		}
		//We can reach here if the number of failed task equal to the number
		//of the nodes, in which case we have nothing to do.

		if (preferred_task)
		{
			return preferred_task->get().second;
		}
		throw std::runtime_error("'execute_on_all_to_figure_out_the_fastest' failed");
	}

	template <typename TResult>
	bool RequestExecutor::handle_unsuccessful_response(std::shared_ptr<const ServerNode> chosen_node,
		const std::optional<int32_t>& node_index, RavenCommand<TResult>& command,
		const impl::CurlHandlesHolder::CurlReference& curl_ref, const impl::CurlResponse& response,
		const std::string& url, const std::optional<documents::session::SessionInfo>& session_info, bool should_retry)
	{
		switch (static_cast<HttpStatus>(response.status_code))
		{
		case HttpStatus::SC_NOT_FOUND:
			//TODO
			//cache.setNotFound(url);
			switch (command.get_response_type())
			{
			case RavenCommandResponseType::EMPTY:
				return true;
			case RavenCommandResponseType::OBJECT:
				command.set_response({}, false);
				break;
			default:
				//TODO
				//command.setResponseRaw(response, null);
				break;
			}
			return true;

		case HttpStatus::SC_FORBIDDEN:
			{
				std::ostringstream msg{};
				msg << "Forbidden access to " << chosen_node->database << "@" << chosen_node->url << ", " <<
					curl_ref.method << " " << curl_ref.url;
				throw exceptions::security::AuthorizationException(msg.str());
			}
		case HttpStatus::SC_GONE:// request not relevant for the chosen node - the database has been moved to a different one
		{
			if (!should_retry)
			{
				return false;
			}
			if (node_index)
			{
				_node_selector->on_failed_request(*node_index);
			}
			if (!command.is_failed_with_node(chosen_node))
			{
				std::ostringstream msg{};
				msg << "Request to " << curl_ref.url << " (" << curl_ref.method << ") is not relevant for this node anymore.";
				command.get_failed_nodes().insert_or_assign(chosen_node, std::make_exception_ptr(
					exceptions::UnsuccessfulRequestException(msg.str())));
			}
			auto index_and_node = choose_node_for_request(command, session_info);

			if (command.get_failed_nodes().find(index_and_node.current_node) != command.get_failed_nodes().end())
			{
				// we tried all the nodes, let's try to update topology and retry one more time
				if (!update_topology_async(chosen_node, 60 * 1000,
					true, "handle-unsuccessful-response", false).get())
				{
					return false;
				}

				command.get_failed_nodes().clear();// we've just updated the topology
				index_and_node = choose_node_for_request(command, session_info);

				execute(index_and_node.current_node, index_and_node.current_index, command, false, session_info);
				return true;
			}

			execute(index_and_node.current_node, index_and_node.current_index, command, false, session_info);
			return true;
		}
		case HttpStatus::SC_GATEWAY_TIMEOUT:
		case HttpStatus::SC_REQUEST_TIMEOUT:
		case HttpStatus::SC_BAD_GATEWAY:
		case HttpStatus::SC_SERVICE_UNAVAILABLE:
			return handle_server_down(chosen_node, node_index, command, curl_ref, response, {}, session_info);
		case HttpStatus::SC_CONFLICT:
			handle_conflict(response);
			break;
		default:
			command.on_response_failure(response);
			exceptions::ExceptionDispatcher::throw_exception(response);
			break;
		}
		return false;
	}

	template <typename TResult>
	bool RequestExecutor::handle_server_down(std::shared_ptr<const ServerNode> chosen_node,
		const std::optional<int32_t>& node_index, RavenCommand<TResult>& command,
		const impl::CurlHandlesHolder::CurlReference& curl_ref, const impl::CurlResponse& response,
		std::exception_ptr e, const std::optional<documents::session::SessionInfo>& session_info)
	{
		add_failed_response_to_command(chosen_node, command, curl_ref, response, e);

		if(!node_index)
		{
			//We executed request over a node not in the topology. This means no failover...
			return false;
		}

		spawn_health_check(chosen_node, *node_index);

		if(!_node_selector)
		{
			return false;
		}

		_node_selector->on_failed_request(*node_index);

		auto current_node_and_index = _node_selector->get_preferred_node();
		if(command.get_failed_nodes().find(current_node_and_index.current_node) !=
			command.get_failed_nodes().end())
		{
			return false; //we tried all the nodes...nothing left to do
		}

		execute(current_node_and_index.current_node, current_node_and_index.current_index, command, false, session_info);

		return true;
	}

	template <typename TResult>
	void RequestExecutor::add_failed_response_to_command(std::shared_ptr<const ServerNode> chosen_node,
		RavenCommand<TResult>& command, const impl::CurlHandlesHolder::CurlReference& curl_ref,
		const impl::CurlResponse& response, std::exception_ptr e)
	{
		const auto& response_json = response.output;
		if (!response_json.empty())
		{
			try
			{
				auto read_exception = exceptions::ExceptionDispatcher::get(
					nlohmann::json::parse(response_json).get<exceptions::ExceptionSchema>(), response.status_code, e);
				command.get_failed_nodes().insert_or_assign(chosen_node, std::make_exception_ptr(read_exception));
			}
			catch (...)
			{
				auto schema = exceptions::ExceptionSchema();
				schema.url = curl_ref.url;
				schema.message = "Got unrecognized response from the server";
				schema.error = response_json;
				schema.type = "Non-ParseableServerResponse";

				auto exception_to_use = exceptions::ExceptionDispatcher::get(schema, response.status_code, e);
				command.get_failed_nodes().insert_or_assign(chosen_node, exception_to_use);
			}
			return;
		}

		// this would be connections that didn't have response, such as "couldn't connect to remote server"
		auto schema = exceptions::ExceptionSchema();
		schema.url = curl_ref.url;
		if (e)
		{
			try
			{
				std::rethrow_exception(e);
			}
			catch (const std::exception& exception)
			{
				schema.message = exception.what();
			}
		}
		std::ostringstream error{};
		error << "An exception occurred while contacting " << curl_ref.url << ".\r\n";
		if (!response.error.empty())
		{
			error << "With Curl error: " << response.error << ".";
		}
		schema.error = error.str();

		command.get_failed_nodes().insert_or_assign(chosen_node, exceptions::ExceptionDispatcher::get(schema,
			static_cast<int32_t>(HttpStatus::SC_SERVICE_UNAVAILABLE), e));
	}

	template <typename TResult>
	void RequestExecutor::execute(RavenCommand<TResult>& command,
		const std::optional<documents::session::SessionInfo>& session_info)
	{
		auto&& topology_update = _first_topology_update;
		bool execute_unlikely = true;
		do
		{
			if (_disable_topology_updates)
			{
				execute_unlikely = false;
			}
			else if(topology_update && topology_update->valid() && 
				topology_update->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
			{
				try
				{
					topology_update->get();
				}
				catch (...)
				{
					break;
				}
				execute_unlikely = false;
			}
		} while(false);

		if(execute_unlikely)
		{
			unlikely_execute(command, topology_update, session_info);
			return;
		}

		auto current_index_and_node = choose_node_for_request(command, session_info);
		execute(current_index_and_node.current_node, current_index_and_node.current_index, command, true, session_info);
	}

	template <typename TResult>
	CurrentIndexAndNode RequestExecutor::choose_node_for_request(RavenCommand<TResult>& cmd,
		const std::optional<documents::session::SessionInfo>& session_info)
	{
		if(!cmd.is_read_request())
		{
			return _node_selector->get_preferred_node();
		}

		switch (get_conventions()->get_read_balance_behavior())
		{
		case ReadBalanceBehavior::NONE:
			return _node_selector->get_preferred_node();
		case ReadBalanceBehavior::ROUND_ROBIN:
			return _node_selector->get_node_by_session_id(session_info ? session_info->session_id : 0);
		case ReadBalanceBehavior::FASTEST_NODE:
			return _node_selector->get_fastest_node();
		default:
			throw std::invalid_argument("");
		}
	}

	template <typename TResult>
	void RequestExecutor::execute(std::shared_ptr<const ServerNode> chosen_node,
		const std::optional<int32_t>& node_index, RavenCommand<TResult>& command, bool should_retry,
		const std::optional<documents::session::SessionInfo>& session_info)
	{
		auto curl_ref = _curl_holder.checkout_curl();
		std::optional<std::string> url_ref{};

		create_request(curl_ref, chosen_node, command, url_ref);

		//TODO implement caching 

		if(!_disable_client_configuration_updates)
		{
			std::ostringstream header;
			header << '"' << client_configuration_etag << '"';
			curl_ref.headers.insert_or_assign(constants::headers::CLIENT_CONFIGURATION_ETAG, header.str());
		}
		if(session_info && session_info->last_cluster_transaction_index)
		{
			curl_ref.headers.insert_or_assign(constants::headers::LAST_KNOWN_CLUSTER_TRANSACTION_INDEX,
				std::to_string(session_info->last_cluster_transaction_index.value()));
		}
		if (!_disable_topology_updates)
		{
			std::ostringstream header;
			header << '"' << topology_etag << '"';
			curl_ref.headers.insert_or_assign(constants::headers::TOPOLOGY_ETAG, header.str());
		}

		auto response = std::optional<impl::CurlResponse>{};
		auto response_dispose = ResponseDisposeHandling::AUTOMATIC;

		try
		{
			++number_of_server_requests;

			if(should_execute_on_all(chosen_node, command))
			{
				response.emplace(execute_on_all_to_figure_out_the_fastest(chosen_node, command));
			}
			else
			{
				response.emplace(command.send(curl_ref));
			}

			if (response->perform_result != CURLE_OK)
			{
				throw exceptions::CurlException(*response);
			}

			if(session_info && session_info->last_cluster_transaction_index)
			{
				// If we reach here it means that sometime a cluster transaction has occurred against this database.
				// Since the current executed command can be dependent on that, we have to wait for the cluster transaction.
				// But we can't do that if the server is an old one.

				if(auto it = response->headers.find(constants::headers::SERVER_VERSION);
					it == response->headers.end() || it->second.compare("4.1") < 0)
				{
					std::ostringstream msg{};
					msg <<
						"The server on " << chosen_node->url <<
						" has an old version and can't perform " <<
						"the command since this command dependent on a cluster transaction which this node doesn't support.";
					throw exceptions::ClientVersionMismatchException(msg.str());
				}
			}
		}
		catch (...)
		{
			if(!should_retry)
			{
				throw;
			}

			if(!handle_server_down(chosen_node, node_index, command, curl_ref, *response,
				std::current_exception(), session_info))
			{
				throw_failed_to_contact_all_nodes(command, curl_ref, std::current_exception(), nullptr);
			}
			return;
		}

		command.status_code = response->status_code;
		bool refresh_topology = extensions::HttpExtensions::get_boolean_header(*response,
			constants::headers::REFRESH_TOPOLOGY).value_or(false);

		bool refresh_client_configuration = extensions::HttpExtensions::get_boolean_header(*response,
			constants::headers::REFRESH_CLIENT_CONFIGURATION).value_or(false);

		const auto finally = [&]()
		{
			if(refresh_topology || refresh_client_configuration)
			{
				auto server_node = std::make_shared<ServerNode>();
				server_node->url = chosen_node->url;
				server_node->database = _database_name.value_or("");

				if(refresh_topology)
				{
					update_topology_async(std::const_pointer_cast<const ServerNode>(server_node), 0,
						false, {}, false).get();
				}
				if(refresh_client_configuration)
				{
					update_client_configuration_async().get();
				}
			}
		};
		try
		{
			//TODO
			//if (response.getStatusLine().getStatusCode() == HttpStatus.SC_NOT_MODIFIED) {
			//	cachedItem.notModified();

			//	try {
			//		if (command.getResponseType() == RavenCommandResponseType.OBJECT) {
			//			command.setResponse(cachedValue.value, true);
			//		}
			//	}
			//	catch (IOException e) {
			//		throw ExceptionsUtils.unwrapException(e);
			//	}

			//	return;
			//}

			if(response->status_code >= 400)
			{
				if(!handle_unsuccessful_response(chosen_node, node_index, command, curl_ref, *response, *url_ref, session_info, should_retry))
				{
					if(auto it = response->headers.find("Database-Missing");
						it != response->headers.end())
					{
						throw exceptions::database::DatabaseDoesNotExistException(it->second);
					}
					if(command.get_failed_nodes().empty())//precaution, should never happen at this point
					{
						throw std::runtime_error("Received unsuccessful response and couldn't recover from it. "
							"Also, no record of exceptions per failed nodes. This is weird and should not happen.");
					}
					if(command.get_failed_nodes().size() == 1 && command.get_failed_nodes().begin()->second)
					{
						std::rethrow_exception(command.get_failed_nodes().begin()->second);
					}
					throw exceptions::AllTopologyNodesDownException(
						"Received unsuccessful response from all servers and couldn't recover from it.");
				}
				finally();
				return;// we either handled this already in the unsuccessful response or we are throwing
			}
			response_dispose = command.process_response(/*cache,*/ *response, *url_ref);
			_last_returned_response = std::chrono::steady_clock::now();
		}
		catch (...)
		{
			finally();
			throw;
		}
		finally();
	}

	//TODO old version -> erase later
	//template<typename TResult>
	//std::shared_ptr<TResult> RequestExecutor::execute(RavenCommand<TResult>& cmd)
	//{
	//	std::optional<std::ostringstream> errors;

	//	std::shared_ptr<Topology> topology_local = std::atomic_load(&_topology);

	//	for (auto& node : topology_local->nodes)
	//	{
	//		try
	//		{
	//			return execute_internal(node, cmd);
	//		}
	//		catch (RavenError& re)
	//		{
	//			if (!errors)
	//			{
	//				errors.emplace();
	//			}
	//			errors.value() << re.what() << '\n';
	//			if (re.get_error_type() == RavenError::ErrorType::DATABASE_DOES_NOT_EXIST)
	//			{
	//				throw RavenError(errors->str(), RavenError::ErrorType::DATABASE_DOES_NOT_EXIST);
	//			}
	//			continue;
	//		}
	//	}
	//	throw RavenError(errors->str(), RavenError::ErrorType::GENERIC);
	//}
}
