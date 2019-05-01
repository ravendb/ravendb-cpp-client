#pragma once
#include <curl/curl.h>
#include <optional>
#include <sstream>
#include "json.hpp"
#include "ServerNode.h"
#include "CurlSListHolder.h"
#include "CurlHandlesHolder.h"
#include "CurlResponse.h"
#include "GetCppClassName.h"

namespace ravendb::client::http
{
	enum class RavenCommandResponseType
	{
		EMPTY,
		OBJECT,
		RAW
	};

	template<typename TResult>
	class RavenCommand //abstract
	{
	public:
		using ResultType = TResult;

		static const std::string result_type_name;

	private:
		std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr> _failed_nodes{};

	protected:
		std::shared_ptr<ResultType> _result{};
		RavenCommandResponseType _response_type = RavenCommandResponseType::OBJECT;
		bool _can_cache = true;
		bool _can_cache_aggressively = true;
		

		RavenCommand() = default;

		static void throw_invalid_response();

	public:
		int32_t status_code = -1;

		virtual ~RavenCommand() = 0;

		static void add_change_vector_if_not_null(impl::CurlHandlesHolder::CurlReference& curl_ref, 
			const std::optional<std::string>& change_vector);

		virtual bool is_read_request() const noexcept = 0;

		RavenCommandResponseType get_response_type() const noexcept;

		std::shared_ptr<ResultType> get_result() const noexcept;

		bool can_cache() const noexcept;

		bool can_cache_aggressively() const noexcept;

		virtual void create_request(CURL* curl, const ServerNode& node, std::optional<std::string>& url_ref) = 0;

		virtual void set_response(CURL* curl, const nlohmann::json& response, bool from_cache = false) = 0;

		virtual void set_response_not_found(CURL* curl);

		static impl::CurlResponse send(const impl::CurlHandlesHolder::CurlReference& curl_ref);

		std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr>& get_failed_nodes();

		bool is_failed_with_node(std::shared_ptr<const ServerNode> node) const;
	};

	template <typename TResult>
	const std::string RavenCommand<TResult>::result_type_name = impl::utils::GetCppClassName()(typeid(ResultType));

	template <typename TResult>
	void RavenCommand<TResult>::throw_invalid_response()
	{
		throw std::runtime_error("Response is invalid");
	}

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;

	template<typename TResult>
	void RavenCommand<TResult>::add_change_vector_if_not_null(impl::CurlHandlesHolder::CurlReference& curl_ref,
		const std::optional<std::string>& change_vector)
	{
		if (change_vector)
		{
			std::ostringstream change_vector_header;
			change_vector_header << '"' << *change_vector << '"';
			curl_ref.headers.insert_or_assign("If-Match", change_vector_header.str());
		}
	}

	template<typename TResult>
	RavenCommandResponseType RavenCommand<TResult>::get_response_type() const noexcept
	{
		return _response_type;
	}

	template<typename TResult>
	std::shared_ptr<TResult> RavenCommand<TResult>::get_result() const noexcept
	{
		return _result;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::can_cache() const noexcept
	{
		return _can_cache;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::can_cache_aggressively() const noexcept
	{
		return _can_cache_aggressively;
	}

	template<typename TResult>
	void RavenCommand<TResult>::set_response_not_found(CURL * curl)
	{
		_response_type = RavenCommandResponseType::EMPTY;
	}

	template <typename TResult>
	impl::CurlResponse RavenCommand<TResult>::send(const impl::CurlHandlesHolder::CurlReference& curl_ref)
	{
		return impl::CurlResponse::run_curl_perform(curl_ref);
	}

	template<typename TResult>
	std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr>& RavenCommand<TResult>::get_failed_nodes()
	{
		return _failed_nodes;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::is_failed_with_node(std::shared_ptr<const ServerNode> node) const
	{
		return _failed_nodes.find(node) != _failed_nodes.end();
	}
}



