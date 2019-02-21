#pragma once
#include <curl/curl.h>
#include <optional>
#include "json.hpp"
#include "ServerNode.h"
#include "CurlSListHolder.h"

namespace ravendb::client::http
{
	enum class RavenCommandResponseType
	{
		EMPTY,
		OBJECT,
	};

	template<typename Result_t>
	class RavenCommand //abstract
	{
	private:
		std::map<ServerNode, std::exception> _failed_nodes;

	protected:
		Result_t _result{};
		RavenCommandResponseType _response_type = RavenCommandResponseType::OBJECT;
		bool _can_cache = true;
		bool _can_cache_aggressively = true;
		impl::CurlSListHolder _headers_list{};

		RavenCommand() = default;

	public:
		int32_t status_code = -1;

		virtual ~RavenCommand() = 0;

		void add_change_vector_if_not_null(CURL* curl, const std::optional<std::string>& change_vector);

		virtual bool is_read_request() const noexcept = 0;

		RavenCommandResponseType get_response_type() const noexcept;

		const Result_t& get_result() const noexcept;

		bool can_cache() const noexcept;

		bool can_cache_aggressively() const noexcept;

		virtual void create_request(CURL* curl, const ServerNode& node, std::string& url) = 0;

		virtual void set_response(CURL* curl, const nlohmann::json& response, bool from_cache = false) = 0;

		virtual void set_response_not_found(CURL* curl);

		const std::map<ServerNode, std::exception>& get_failed_nodes() const noexcept;

		bool is_failed_with_node(const ServerNode& node) const noexcept;
	};

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;

	template<typename Result_t>
	void RavenCommand<Result_t>::add_change_vector_if_not_null(CURL * curl, const std::optional<std::string>& change_vector)
	{
		if (change_vector)
		{
			std::ostringstream change_vector_header;
			change_vector_header << "If-Match:" << '"' << *change_vector << '"';
			_headers_list.append(change_vector_header.str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list.get());
		}
	}

	template<typename Result_t>
	RavenCommandResponseType RavenCommand<Result_t>::get_response_type() const noexcept
	{
		return _response_type;
	}

	template<typename Result_t>
	const Result_t & RavenCommand<Result_t>::get_result() const noexcept
	{
		return _result;
	}

	template<typename Result_t>
	bool RavenCommand<Result_t>::can_cache() const noexcept
	{
		return _can_cache;
	}

	template<typename Result_t>
	bool RavenCommand<Result_t>::can_cache_aggressively() const noexcept
	{
		return _can_cache_aggressively;
	}

	template<typename Result_t>
	void RavenCommand<Result_t>::set_response_not_found(CURL * curl)
	{
		_response_type = RavenCommandResponseType::EMPTY;
	}

	template<typename Result_t>
	const std::map<ServerNode, std::exception>& RavenCommand<Result_t>::get_failed_nodes() const noexcept
	{
		return _failed_nodes;
	}

	template<typename Result_t>
	bool RavenCommand<Result_t>::is_failed_with_node(const ServerNode & node) const noexcept
	{
		return _failed_nodes.find(node) != _failed_nodes.end();
	}

}



