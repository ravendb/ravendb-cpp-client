#pragma once


#include "stdafx.h"
#include "utils.h"

namespace raven
{
	enum class RavenCommandResponseType
	{
		EMPTY,
		OBJECT,
	};


	template<typename Result_t>
	class RavenCommand
	{
	private:
		std::map<ServerNode, std::exception> _failed_nodes;

	protected:
		Result_t _result;
		RavenCommandResponseType _response_type = RavenCommandResponseType::OBJECT;
		bool _can_cache = true;
		bool _can_cache_aggressively = true;

	public:
		int32_t status_code = -1;

		virtual ~RavenCommand() = 0;

		virtual bool is_read_request() const noexcept = 0;

		RavenCommandResponseType get_response_type() const noexcept
		{
			return _response_type;
		}

		const Result_t& get_result() const noexcept
		{
			return _result;
		}

		bool can_cache() const noexcept
		{
			return _can_cache;
		}

		bool can_cache_aggressively() const noexcept
		{
			return _can_cache_aggressively;
		}

		virtual void create_request(CURL* curl, const ServerNode& node, std::string& url) const = 0;

		virtual void set_response(CURL* curl, const nlohmann::json& response, bool from_cache = false) = 0;

		//do we need it ?
		virtual void set_response_not_found(CURL* curl)
		{
			/*_result = TResult();*/
		}

		//do we need it ? should we use it ?
		//public CloseableHttpResponse send(CloseableHttpClient client, HttpRequestBase request) throws IOException {
		//	return client.execute(request);
		//}

		const std::map<ServerNode, std::exception>& get_failed_nodes() const noexcept
		{
			return _failed_nodes;
		}

		bool is_failed_with_node(const ServerNode& node) const noexcept
		{
			return _failed_nodes.find(node) != _failed_nodes.cend();
		}

	};

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;
}



