#pragma once
#include <chrono>
#include <string>
#include <memory>

namespace ravendb::client::http
{
	class HttpCache;

	struct HttpCacheItem
	{
		friend class HttpCache;
	private:
		//for cache eviction algorithm use ONLY !!!
		std::chrono::steady_clock::time_point last_access{};

	public:
		std::string change_vector{};
		std::string payload{};
		std::chrono::steady_clock::time_point last_sever_update{};
		int32_t generation{};

		std::weak_ptr<HttpCache> cache{};

	public:
		HttpCacheItem()
			: last_access(std::chrono::steady_clock::now())
			, last_sever_update(std::chrono::steady_clock::now())
		{}
	};
}
