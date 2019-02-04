#pragma once
#include "DocumentInfo.h"
#include "DocumentsByIdsMap.h"

namespace ravendb::client::documents::session
{
	class DocumentSessionImpl;
}

namespace ravendb::client::documents::session::loaders
{
	class LoaderWithInclude;

	class MultiLoaderWithInclude
	{
	private:
		std::reference_wrapper<DocumentSessionImpl> _session;
		std::vector<std::string> _includes{};
		std::weak_ptr<MultiLoaderWithInclude> _weak_this{};

		explicit MultiLoaderWithInclude(DocumentSessionImpl& session)
			: _session(session)
		{}

	public:
		~MultiLoaderWithInclude() = default;

		static LoaderWithInclude create(DocumentSessionImpl& session);

		LoaderWithInclude include(std::string path);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			std::vector<std::reference_wrapper<const std::string>> ids = { cref(id) };
			auto res = _session.get().load_internal<T>(ids.begin(), ids.end(), _includes, from_json, to_json);
			for(const auto& [id, entity] : res)
			{
				if(entity)
				{
					return entity;
				}
			}
			return nullptr;
		}

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>, "invalid iterator type");

			std::vector<std::reference_wrapper<const std::string>> ids{};
			std::transform(first, last, std::back_inserter(ids), [](const std::string& id) {return std::cref(id); });

			return _session.get().load_internal<T>(ids, _includes, from_json, to_json);
		}
	};
}
