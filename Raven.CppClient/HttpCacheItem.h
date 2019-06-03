#pragma once

namespace ravendb::client::http
{
	class HttpCache;

	struct HttpCacheItem
	{
		std::string change_vector{};
		std::string payload{};
		std::chrono::steady_clock::time_point last_sever_update{};
		int32_t generation{};

		std::weak_ptr<HttpCache> cache{};

		HttpCacheItem()
			: last_sever_update(std::chrono::steady_clock::now())
		{}
	};
}
