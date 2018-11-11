#pragma once

#include "stdafx.h"
#include <queue>

namespace ravendb::client::impl
{
	// used when user wants to customize curl settings
	//(like using Fiddler)
	using CreateCurlHandleMethod_t = CURL * (*)(void*);

	class CurlHandlesHolder
	{
	private:
		struct CurlGlobalInit final
		{
			CurlGlobalInit()
			{
				auto res = curl_global_init(CURL_GLOBAL_ALL);
				if (res not_eq CURLE_OK)
				{
					throw std::runtime_error("curl_global_init failed !");
				}
			}

			~CurlGlobalInit()
			{
				curl_global_cleanup();
			}

			CurlGlobalInit(const CurlGlobalInit& other) = delete;
			CurlGlobalInit(CurlGlobalInit&& other) noexcept = delete;
			CurlGlobalInit& operator=(const CurlGlobalInit& other) = delete;
			CurlGlobalInit& operator=(CurlGlobalInit&& other) noexcept = delete;
		};

		inline static CurlGlobalInit _cgr{};

		struct CurlCleanup
		{
			void operator ()(CURL* curl) const { curl_easy_cleanup(curl); }
		};

		using CurlHandleUniquePtr = std::unique_ptr<CURL, CurlCleanup>;

		CreateCurlHandleMethod_t _create_curl_handle_method = default_create_curl_instance;
		void* _state = nullptr;

		std::mutex _curl_handles_lock{};

		// protected by _curl_handles_lock
		std::queue<CurlHandleUniquePtr> _curl_handles{};

		void return_curl_handle_to_holder(CurlHandleUniquePtr curl)
		{
			std::lock_guard<std::mutex> lg(_curl_handles_lock);
			_curl_handles.push(std::move(curl));
		}

	public:
		static CURL* default_create_curl_instance(void* state)
		{
			return curl_easy_init();
		}

		class ReturnCurl
		{
			friend CurlHandlesHolder;
		private:
			CurlHandleUniquePtr _curl;
			CurlHandlesHolder* _holder;

			ReturnCurl(CurlHandleUniquePtr curl, CurlHandlesHolder& h) noexcept
				: _curl(std::move(curl))
				, _holder(&h)
			{}

		public:
			ReturnCurl(const ReturnCurl& other) = delete;
			ReturnCurl& operator=(const ReturnCurl& other) = delete;
			ReturnCurl& operator=(ReturnCurl&& other) = delete;

			ReturnCurl(ReturnCurl&& other) = delete;/*noexcept
				: _curl(std::move(other._curl))
				, _holder(other._holder)
			{
				_holder = nullptr;
			}*/

			~ReturnCurl()
			{
				curl_easy_reset(this->get());
				_holder->return_curl_handle_to_holder(std::move(_curl));
			}

			CURL* get() const
			{
				if (_curl)
				{
					return _curl.get();
				}else
				{
					throw std::runtime_error("ReturnCurl object does not contain valid curl handle");
				}
			}
		};

		ReturnCurl checkout_curl()
		{
			std::lock_guard<std::mutex> lg(_curl_handles_lock);
			if (_curl_handles.empty())
			{
				auto curl = _create_curl_handle_method(_state);
				if (curl == nullptr)
				{
					throw std::runtime_error("_create_curl_handle_method failed");
				}
				return { CurlHandleUniquePtr(curl), *this };
			}

			auto curl = std::move(_curl_handles.front());
			_curl_handles.pop();
			return { std::move(curl), *this };
		}

		CurlHandlesHolder()
			: CurlHandlesHolder(default_create_curl_instance, nullptr)
		{}

		CurlHandlesHolder(CreateCurlHandleMethod_t create_curl_instance_method, void* state)
			: _create_curl_handle_method(create_curl_instance_method)
			, _state(state)
		{}
	};
}