#pragma once
#include "DocumentInfo.h"
#include "DocumentsByIdsMap.h"
#include "DocumentSessionImpl.h"

namespace ravendb::client::documents::session::loaders
{
	class LoaderWithInclude;

	class MultiLoaderWithInclude
	{
	private:
		std::shared_ptr<DocumentSessionImpl> _session;
		std::vector<std::string> _includes{};
		std::weak_ptr<MultiLoaderWithInclude> _weak_this{};

		explicit MultiLoaderWithInclude(std::shared_ptr<DocumentSessionImpl> session)
			: _session(session)
		{}

	public:
		~MultiLoaderWithInclude() = default;

		static LoaderWithInclude create(std::shared_ptr<DocumentSessionImpl> session);

		LoaderWithInclude include(std::string path);

		template<typename T>
		std::shared_ptr<T> load(
			const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});
	};

	template <typename T>
	std::shared_ptr<T> MultiLoaderWithInclude::load(
		const std::string& id,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		std::vector<std::reference_wrapper<const std::string>> ids = {cref(id)};
		auto res = _session->load_internal<T>(ids, _includes, from_json, to_json);
		for (const auto& [id, entity] : res)
		{
			if (entity)
			{
				return entity;
			}
		}
		return nullptr;
	}

	template <typename T, typename InputIt>
	DocumentsByIdsMap<T> MultiLoaderWithInclude::load(InputIt first, InputIt last,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		static_assert(std::is_same_v<std::remove_cv_t<typename std::iterator_traits<InputIt>::value_type>, std::string>,
			"invalid iterator type");

		std::vector<std::reference_wrapper<const std::string>> ids{};
		std::transform(first, last, std::back_inserter(ids), [](const std::string& id) { return std::cref(id); });

		return _session->load_internal<T>(ids, _includes, from_json, to_json);
	}
}
