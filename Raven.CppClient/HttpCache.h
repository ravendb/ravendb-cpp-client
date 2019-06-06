#pragma once
#include <unordered_map>
#include <atomic>
#include <shared_mutex>
#include "HttpCacheItem.h"

namespace ravendb::client::http
{
	class HttpCache
	{
	public:
		struct ReleaseCacheItem
		{
			const std::shared_ptr<HttpCacheItem> item;

		public:
			explicit ReleaseCacheItem(std::shared_ptr<HttpCacheItem> item);

			void not_modified();

			std::chrono::milliseconds get_age() const;

			bool get_might_have_been_modified() const;
		};

	private:
		std::weak_ptr<HttpCache> _weak_this{};

		const uint64_t _max_size{};

		std::unordered_map<std::string, std::shared_ptr<HttpCacheItem>> _items{};
		mutable std::shared_mutex _global_mutex{};
		std::mutex _cleaner_mutex{};

		std::atomic_uint64_t _total_size{};

		std::atomic_flag _is_free_space_running{ false };

	public:
		std::atomic_int32_t generation{ 0 };

	private:
		explicit HttpCache(uint64_t max_size);

		void free_space();

	public:
		//max_size is the maximum size of cached responses(strings), not cached items
		static std::shared_ptr<HttpCache> create(uint64_t max_size);

		std::size_t get_number_of_items() const;

		void set(std::string url, std::string change_vector, std::string result);

		void set_not_found(std::string url);

		ReleaseCacheItem get(const std::string& url,
			std::optional<std::string>& change_vector_ref,
			std::optional<std::string>& response_ref) const;
	};
}
