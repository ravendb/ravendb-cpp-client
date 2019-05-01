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

		class CurlReference
		{
			friend CurlHandlesHolder;
		private:
			CurlHandleUniquePtr _curl;

			CurlHandlesHolder* _holder;

		public:
			std::string url{};
			std::string method{};
			std::unordered_map<std::string, std::string> headers{};

			std::optional<CurlOptionsSetter> set_before_perform{};
			std::optional<CurlOptionsSetter> set_after_perform{};

		private:
			CurlReference(CurlHandleUniquePtr curl, CurlHandlesHolder& h);

		public:
			CURL* get() const;

			CurlReference(CurlReference&& other) = delete;
			CurlReference& operator=(CurlReference&& other) = delete;

			~CurlReference();
		};

		CurlReference checkout_curl();
	};
}