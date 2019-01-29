#pragma once
#include <curl/curl.h>
#include <queue>
#include <functional>
#include <mutex>

namespace ravendb::client::impl
{
	// used when user wants to customize curl settings
	//(like using Fiddler)
	using CurlOptionsSetter = std::function<void(CURL*)>;

	class CurlHandlesHolder
	{
	private:
		struct CurlGlobalInit
		{
			CurlGlobalInit();

			~CurlGlobalInit();
		};

		static CurlGlobalInit _curl_global_init;

		struct CurlCleanup
		{
			void operator ()(CURL* curl) const { curl_easy_cleanup(curl); }
		};

		using CurlHandleUniquePtr = std::unique_ptr<CURL, CurlCleanup>;

		std::mutex _curl_handles_lock{};
		// protected by _curl_handles_lock
		std::queue<CurlHandleUniquePtr> _curl_handles{};

		void return_curl_handle_to_holder(CurlHandleUniquePtr curl);

	public:
		CurlHandlesHolder() = default;
		~CurlHandlesHolder() = default;

		class ReturnCurl
		{
			friend CurlHandlesHolder;
		private:
			CurlHandleUniquePtr _curl;
			CurlHandlesHolder* _holder;

			ReturnCurl(CurlHandleUniquePtr curl, CurlHandlesHolder& h)
				: _curl(std::move(curl))
				, _holder(&h)
			{}

		public:
			ReturnCurl(const ReturnCurl& other) = delete;
			ReturnCurl(ReturnCurl&& other) = delete;
			ReturnCurl& operator=(const ReturnCurl& other) = delete;
			ReturnCurl& operator=(ReturnCurl&& other) = delete;

			~ReturnCurl();

			CURL* get() const;
		};

		ReturnCurl checkout_curl();
	};
}