#include "stdafx.h"
#include "HttpCache.h"
#include <future>

namespace ravendb::client::http
{
	HttpCache::ReleaseCacheItem::ReleaseCacheItem(std::shared_ptr<HttpCacheItem> item)
		: item(item)
	{}

	void HttpCache::ReleaseCacheItem::not_modified()
	{
		if(item)
		{
			item->last_sever_update = std::chrono::steady_clock::now();
		}
	}

	std::chrono::milliseconds HttpCache::ReleaseCacheItem::get_age() const
	{
		if(!item)
		{
#ifdef max
#undef max
			return std::chrono::milliseconds::max();
#endif
		}
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - item->last_sever_update);
	}

	bool HttpCache::ReleaseCacheItem::get_might_have_been_modified() const
	{
		return item->generation != item->cache.lock()->generation.load();
	}

	HttpCache::HttpCache(uint64_t max_size)
		: _max_size(max_size)
	{}

	void HttpCache::free_space()
	{
		if(_is_free_space_running.test_and_set())
		{
			return;
		}

		struct ClearFreeSpaceRunningFlag
		{
			HttpCache& _http_cache;

			~ClearFreeSpaceRunningFlag()
			{
				_http_cache._is_free_space_running.clear();
			}
		} clear_flag{ *this };

		auto cleaner_lock = std::lock_guard(_cleaner_mutex);

		std::size_t size_to_clear{};
		std::size_t number_of_cleared_items{ 0 };//for log/debugging
		uint64_t size_cleared{ 0 };
		std::multimap<std::chrono::steady_clock::time_point, decltype(_items)::iterator> items_its{};
		
		{
			auto gl = std::shared_lock(_global_mutex);

			if (_items.empty())
			{
				return;
			}

			for(auto it = _items.begin(); it != _items.end(); ++it)
			{
				items_its.emplace(it->second->last_access, it);
				size_to_clear += it->second->payload.size() * sizeof(std::string::value_type);
			}
			size_to_clear /= 2;
		}

		auto gl = std::unique_lock(_global_mutex);
		const auto start = std::chrono::steady_clock::now();

		for(const auto& [timestamp, iterator] : items_its)
		{
			if (size_cleared > size_to_clear &&
				std::chrono::duration_cast<std::chrono::minutes>(start - iterator->second->last_sever_update).count() <= 1)
			{
				continue;
			}

			++number_of_cleared_items;
			size_cleared += iterator->second->payload.size() * sizeof(std::string::value_type);
			_items.erase(iterator);
		}
	}

	std::shared_ptr<HttpCache> HttpCache::create(uint64_t max_size)
	{
		auto object = std::shared_ptr<HttpCache>(new HttpCache(max_size));
		object->_weak_this = object;
		return object;
	}

	std::size_t HttpCache::get_number_of_items() const
	{
		auto lock = std::shared_lock(_global_mutex);
		return _items.size();
	}

	void HttpCache::set(std::string url, std::string change_vector, std::string result)
	{
		if (_total_size += result.size() * sizeof(std::string::value_type) > _max_size)
		{
			auto clean = std::async(std::launch::async, [http_cache = _weak_this.lock()]()->void
			{
				if (http_cache)
				{
					http_cache->free_space();
				}
			});
		}

		auto http_cache_item = std::make_shared<HttpCacheItem>();
		http_cache_item->change_vector = std::move(change_vector);
		http_cache_item->payload = std::move(result);
		http_cache_item->cache = _weak_this;
		http_cache_item->generation = generation.load();

		{
			auto gl = std::unique_lock(_global_mutex, std::defer_lock);
			auto lock = std::scoped_lock(_cleaner_mutex, gl);

			_items.insert_or_assign(std::move(url), http_cache_item);
			http_cache_item->last_access = std::chrono::steady_clock::now();
		}
	}

	void HttpCache::set_not_found(std::string url)
	{
		auto http_cache_item = std::make_shared<HttpCacheItem>();
		http_cache_item->change_vector = "404 response";
		http_cache_item->cache = _weak_this;
		http_cache_item->generation = generation.load();

		{
			auto gl = std::unique_lock(_global_mutex, std::defer_lock);
			auto lock = std::scoped_lock(_cleaner_mutex, gl);

			_items.insert_or_assign(std::move(url), http_cache_item);
			http_cache_item->last_access = std::chrono::steady_clock::now();
		}
	}

	HttpCache::ReleaseCacheItem HttpCache::get(const std::string& url, std::optional<std::string>& change_vector_ref,
		std::optional<std::string>& response_ref) const
	{
		{
			auto lock = std::shared_lock(_global_mutex);

			if (auto it = _items.find(url);
				it != _items.end())
			{
				change_vector_ref.emplace(it->second->change_vector);
				response_ref.emplace(it->second->payload);
				it->second->last_access = std::chrono::steady_clock::now();
				return ReleaseCacheItem(it->second);
			}
		}

		change_vector_ref.reset();
		response_ref.reset();
		return ReleaseCacheItem({});
	}
}
