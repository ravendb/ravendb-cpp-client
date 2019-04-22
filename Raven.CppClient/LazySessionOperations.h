#pragma once
#include "DocumentInfo.h"
#include "LazySessionOperationsImpl.h"

namespace ravendb::client::documents::session::operations::lazy
{
	class LazySessionOperations
	{
	private:
		std::shared_ptr<LazySessionOperationsImpl> _lazy_session_operations_impl;

	public:
		~LazySessionOperations() = default;

		explicit LazySessionOperations(std::shared_ptr<LazySessionOperationsImpl> lazy_session_operations_impl)
			: _lazy_session_operations_impl(lazy_session_operations_impl)
		{}


		loaders::LazyLoaderWithInclude include(const std::string& path)
		{
			return _lazy_session_operations_impl->include(path);
		}

		template<typename T>
		Lazy<std::shared_ptr<T>> load(const std::string& id,
			std::optional<std::function<void(const std::shared_ptr<T>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_session_operations_impl->load<T>(id, on_eval, from_json, to_json);
		}

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		Lazy<DocumentsByIdsMap<T>> load(InputIt ids_first, InputIt ids_last,
			std::optional<std::function<void(const DocumentsByIdsMap<T>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_session_operations_impl->load<T>(ids_first, ids_last, on_eval, from_json, to_json);
		}

		//load by vector of ids (std::string)
		template<typename T>
		Lazy<DocumentsByIdsMap<T>> load(const std::vector<std::string>& ids,
			std::optional<std::function<void(const DocumentsByIdsMap<T>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_session_operations_impl->load<T>(ids.begin(), ids.end(), on_eval, from_json, to_json);
		}

		template<typename T>
		Lazy<DocumentsByIdsMap<T>> load_starting_with(std::string id_prefix,
			std::optional<std::string> matches = {},
			int32_t start = 0,
			int32_t page_size = 25,
			std::optional<std::string> exclude = {},
			std::optional<std::string> start_after = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			return _lazy_session_operations_impl->load_starting_with<T>(
				std::move(id_prefix),
				std::move(matches),
				start,
				page_size,
				std::move(exclude),
				std::move(start_after),
				from_json, to_json);
		}
	};
}
