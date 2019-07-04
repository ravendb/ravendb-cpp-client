#pragma once
#include "DocumentInfo.h"
#include "DocumentsByIdsMap.h"
#include "MultiLoaderWithInclude.h"

namespace ravendb::client::documents::session::loaders
{
	class LoaderWithInclude
	{
	private:
		std::shared_ptr<MultiLoaderWithInclude> _multi_loader_with_include;

	public:
		~LoaderWithInclude() = default;

		explicit LoaderWithInclude(std::shared_ptr<MultiLoaderWithInclude> multi_loader_with_include)
			: _multi_loader_with_include(multi_loader_with_include)
		{}

		LoaderWithInclude include(const std::string& path)
		{
			return _multi_loader_with_include->include(path);
		}

		template<typename T>
		std::shared_ptr<T> load(const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _multi_loader_with_include->load<T>(id, from_json, to_json);
		}

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _multi_loader_with_include->load<T, InputIt>(first, last, from_json, to_json);
		}

		//load by collections of ids (std::string)
		template<typename T>
		DocumentsByIdsMap<T> load(const std::vector<std::string> ids,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _multi_loader_with_include->load<T>(
				ids.begin(), ids.end(), from_json, to_json);
		}
	};
}
