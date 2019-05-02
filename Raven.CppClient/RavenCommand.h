#pragma once
#include <curl/curl.h>
#include <optional>
#include <sstream>
#include "json.hpp"
#include "ServerNode.h"
#include "CurlHandlesHolder.h"
#include "CurlResponse.h"
#include "GetCppClassName.h"
#include "ResponseDisposeHandling.h"
#include "HttpStatus.h"

namespace ravendb::client::http
{
	enum class RavenCommandResponseType
	{
		EMPTY,
		OBJECT,
		RAW
	};

	template<typename TResult>
	class RavenCommand
	{
	public:
		using ResultType = TResult;

	private:
		std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr,
			std::hash<std::shared_ptr<const ServerNode>>, CompareSharedPtrConstServerNode> _failed_nodes{};

	protected:
		std::shared_ptr<ResultType> _result{};
		RavenCommandResponseType _response_type{ RavenCommandResponseType::OBJECT };
		bool _can_cache{ true };
		bool _can_cache_aggressively{ true };

	public:
		static const std::string result_type_name;
		int32_t status_code{};

	protected:
		RavenCommand() = default;

		static std::string url_encode(const impl::CurlHandlesHolder::CurlReference& curl_ref, const std::string& value);

		//TODO
		//void cacheResponse(HttpCache cache, String url, CloseableHttpResponse response, String responseJson)

		static void throw_invalid_response();

		static void throw_invalid_response(std::exception_ptr e);

		static void add_change_vector_if_not_null(impl::CurlHandlesHolder::CurlReference& curl_ref,
			const std::optional<std::string>& change_vector);

		//expects (const) std::string* in stream
		static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);

	public:
		virtual ~RavenCommand() = 0;

		virtual bool is_read_request() const = 0;

		RavenCommandResponseType get_response_type() const;

		std::shared_ptr<ResultType> get_result() const;

		void set_result(std::shared_ptr<ResultType> result);

		bool can_cache() const;

		bool can_cache_aggressively() const;

		virtual void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const ServerNode> node,
			std::optional<std::string>& url_ref) = 0;

		virtual void set_response(const std::optional<nlohmann::json>& response, bool from_cache);

		static impl::CurlResponse send(const impl::CurlHandlesHolder::CurlReference& curl_ref);

		void set_response_raw(const impl::CurlResponse& response);
		
		std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr,
			std::hash<std::shared_ptr<const ServerNode>>, CompareSharedPtrConstServerNode>& get_failed_nodes();
		
		void set_failed_nodes(std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr,
			std::hash<std::shared_ptr<const ServerNode>>, CompareSharedPtrConstServerNode> failed_nodes);

		bool is_failed_with_node(std::shared_ptr<const ServerNode> node) const;
		
		ResponseDisposeHandling process_response(/*HttpCache*/ const impl::CurlResponse& response, const std::string& url);

		void on_response_failure(const impl::CurlResponse& response){}
	};

	template <typename TResult>
	const std::string RavenCommand<TResult>::result_type_name = impl::utils::GetCppClassName()(typeid(ResultType));

	template <typename TResult>
	std::string RavenCommand<TResult>::url_encode(const impl::CurlHandlesHolder::CurlReference& curl_ref,
		const std::string& value)
	{
		char* result = curl_easy_escape(curl_ref.get(), value.c_str(), static_cast<int>(value.length()));

		if (result == nullptr)
		{
			throw std::invalid_argument("url escape failed on: " + value);
		}

		auto str = std::string(result);
		curl_free(result);

		return str;
	}

	template <typename TResult>
	void RavenCommand<TResult>::throw_invalid_response()
	{
		throw std::runtime_error("Response is invalid");
	}

	template <typename TResult>
	void RavenCommand<TResult>::throw_invalid_response(std::exception_ptr e)
	{
		try
		{
			std::rethrow_exception(e);
		}
		catch (std::exception& ex)
		{
			std::throw_with_nested(std::runtime_error("Response is invalid : " + std::string(ex.what())));
		}
	}

	template <typename TResult>
	RavenCommand<TResult>::~RavenCommand() = default;

	template <typename TResult>
	void RavenCommand<TResult>::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		switch (_response_type)
		{
		case RavenCommandResponseType::EMPTY:
		case RavenCommandResponseType::RAW:
			throw_invalid_response();
		case RavenCommandResponseType::OBJECT:
			//TODO
			//throw new UnsupportedOperationException(responseType.name() + " command must override the setResponse method which expects response with the following type: " + responseType);
			throw std::runtime_error("The command must override the 'set_response'"
				" method which expects response with the following type: OBJECT");
		}
	}

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

	template <typename TResult>
	size_t RavenCommand<TResult>::read_callback(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		const auto str = static_cast<const std::string*>(stream);
		const size_t length = str->length();

		std::memcpy(ptr, str->data(), length);
		return length;
	}

	template<typename TResult>
	RavenCommandResponseType RavenCommand<TResult>::get_response_type() const
	{
		return _response_type;
	}

	template<typename TResult>
	std::shared_ptr<TResult> RavenCommand<TResult>::get_result() const
	{
		return _result;
	}

	template <typename TResult>
	void RavenCommand<TResult>::set_result(std::shared_ptr<ResultType> result)
	{
		_result = result;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::can_cache() const
	{
		return _can_cache;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::can_cache_aggressively() const
	{
		return _can_cache_aggressively;
	}

	template <typename TResult>
	impl::CurlResponse RavenCommand<TResult>::send(const impl::CurlHandlesHolder::CurlReference& curl_ref)
	{
		return impl::CurlResponse::run_curl_perform(curl_ref);
	}

	template <typename TResult>
	void RavenCommand<TResult>::set_response_raw(const impl::CurlResponse& response)
	{
		//TODO
		//throw new UnsupportedOperationException("When " + responseType + " is set to Raw then please override this method to handle the response. ")
		throw std::runtime_error("When 'response_type' is set to RAW then please override this method to handle the response.");
	}

	template<typename TResult>
	std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr,
		std::hash<std::shared_ptr<const ServerNode>>, CompareSharedPtrConstServerNode>& RavenCommand<TResult>::get_failed_nodes()
	{
		return _failed_nodes;
	}

	template <typename TResult>
	void RavenCommand<TResult>::set_failed_nodes(
		std::unordered_map<std::shared_ptr<const ServerNode>, std::exception_ptr, std::hash<std::shared_ptr<const
		ServerNode>>, CompareSharedPtrConstServerNode> failed_nodes)
	{
		_failed_nodes = failed_nodes;
	}

	template<typename TResult>
	bool RavenCommand<TResult>::is_failed_with_node(std::shared_ptr<const ServerNode> node) const
	{
		return _failed_nodes.find(node) != _failed_nodes.end();
	}

	template <typename TResult>
	ResponseDisposeHandling RavenCommand<TResult>::process_response(const impl::CurlResponse& response,
		const std::string& url)
	{
		if(response.output.empty())
		{
			return ResponseDisposeHandling::AUTOMATIC;
		}

		if(_response_type == RavenCommandResponseType::EMPTY || response.status_code == (int32_t)HttpStatus::SC_NO_CONTENT)
		{
			return ResponseDisposeHandling::AUTOMATIC;
		}

		if (_response_type == RavenCommandResponseType::OBJECT)
		{

			auto json = nlohmann::json::parse(response.output);
			//TODO
			//if (cache != null) //precaution
			//{
			//	cacheResponse(cache, url, response, json);
			//}
			set_response(std::move(json), false);
			return ResponseDisposeHandling::AUTOMATIC;

		}else
		{
			set_response_raw(response);
		}

		return ResponseDisposeHandling::AUTOMATIC;
	}
}



