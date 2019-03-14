#pragma once
#include "DocumentsByIdsMap.h"
#include "DocumentSessionImpl.h"

namespace ravendb::client::documents::session::loaders
{
	class LazyLoaderWithInclude;

	class LazyMultiLoaderWithInclude
	{
	private:
		std::shared_ptr<DocumentSessionImpl> _session;
		std::vector<std::string> _includes{};
		std::weak_ptr<LazyMultiLoaderWithInclude> _weak_this{};

		explicit LazyMultiLoaderWithInclude(std::shared_ptr<DocumentSessionImpl> session)
			: _session(session)
		{}

	public:
		~LazyMultiLoaderWithInclude() = default;

		static LazyLoaderWithInclude create(std::shared_ptr<DocumentSessionImpl> session);

		LazyLoaderWithInclude include(std::string path);

		template<typename T>
		Lazy<std::shared_ptr<T>> load(const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		Lazy<DocumentsByIdsMap<T>> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});
	};

	template <typename T>
	Lazy<std::shared_ptr<T>> LazyMultiLoaderWithInclude::load(const std::string& id,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		auto id_ref = std::vector<std::reference_wrapper<const std::string>>{std::cref(id)};
		Lazy<DocumentsByIdsMap<T>> lazy_results = _session->lazy_load_internal<T>(id_ref, _includes, {}, from_json, to_json);

		return Lazy<std::shared_ptr<T>>([=]()mutable -> std::shared_ptr<T>
		{
			if (auto it = lazy_results.get_value().find(id);
				it != lazy_results.get_value().end())
			{
				return it->second;
			}
			return nullptr;
		});
	}

	template <typename T, typename InputIt>
	Lazy<DocumentsByIdsMap<T>> LazyMultiLoaderWithInclude::load(InputIt first, InputIt last,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>,
			"invalid iterator type");

		std::vector<std::reference_wrapper<const std::string>> ids{};
		std::transform(first, last, std::back_inserter(ids), [](const std::string& id) { return std::cref(id); });

		return _session->lazy_load_internal<T>(ids, _includes, {}, from_json, to_json);
	}
}
