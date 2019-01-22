#pragma once

#include "stdafx.h"
#include "ServerNode.h"
#include "CurslSListHolder.h"

namespace ravendb::client::http
{
	enum class RavenCommandResponseType
	{
		EMPTY,
		OBJECT,
	};

	//TODO consider using base non_template abstract class/interface
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
		impl::CurlSlistHolder _headers_list{};

		RavenCommand()
			: _response_type(RavenCommandResponseType::OBJECT)
			, _can_cache(true)
			, _can_cache_aggressively(true)
			, _headers_list()
		{}

	public:
		int32_t status_code = -1;

		virtual ~RavenCommand() = 0;

		void add_change_vector_if_not_null(CURL* curl, const std::optional<std::string>& change_vector)
		{
			if (change_vector)
			{
				std::ostringstream change_vector_header;
				change_vector_header << "If-Match:" << '"' << *change_vector << '"';
				_headers_list.append(change_vector_header.str());
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list.get());
			}
		}

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

		virtual void create_request(CURL* curl, const ServerNode& node, std::string& url) = 0;

		virtual void set_response(CURL* curl, const nlohmann::json& response, bool from_cache = false) = 0;

		virtual void set_response_not_found(CURL* curl)
		{
			_response_type = RavenCommandResponseType::EMPTY;
		}

		const std::map<ServerNode, std::exception>& get_failed_nodes() const noexcept
		{
			return _failed_nodes;
		}

		bool is_failed_with_node(const ServerNode& node) const noexcept
		{
			return _failed_nodes.find(node) != _failed_nodes.end();
		}

	};

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;
}



