#pragma once
#include "DocumentInfo.h"
#include "DocumentsByIdsMap.h"
#include "LazyMultiLoaderWithInclude.h"

namespace ravendb::client::documents::session::loaders
{
	class LazyLoaderWithInclude
	{
	private:
		std::shared_ptr<LazyMultiLoaderWithInclude> _lazy_multi_loader_with_include;

	public:
		~LazyLoaderWithInclude() = default;

		explicit LazyLoaderWithInclude(std::shared_ptr<LazyMultiLoaderWithInclude> lazy_multi_loader_with_include)
			: _lazy_multi_loader_with_include(lazy_multi_loader_with_include)
		{}

		LazyLoaderWithInclude include(const std::string& path)
		{
			return _lazy_multi_loader_with_include->include(path);
		}

		template<typename T>
		Lazy<std::shared_ptr<T>> load(const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_multi_loader_with_include->load<T>(id, from_json, to_json);
		}

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		Lazy<DocumentsByIdsMap<T>> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_multi_loader_with_include->load<T, InputIt>(first, last, from_json, to_json);
		}
	};
}
