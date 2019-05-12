#pragma once
#include "DocumentInfo.h"
#include "DocumentsByIdsMap.h"
#include "Lazy.h"
#include "DocumentSessionImpl.h"
#include "LazyLoadOperation.h"
#include "LazyMultiLoaderWithInclude.h"
#include "LazyLoaderWithInclude.h"
#include "LazyStartsWithOperation.h"

namespace ravendb::client::documents::session::operations::lazy
{
	class LazySessionOperations;

	class LazySessionOperationsImpl
	{
	private:
		std::shared_ptr<DocumentSessionImpl> _session;
		std::weak_ptr<LazySessionOperationsImpl> _weak_this{};

		explicit LazySessionOperationsImpl(std::shared_ptr<DocumentSessionImpl> session)
			: _session(session)
		{}

	public:
		~LazySessionOperationsImpl() = default;

		static LazySessionOperations create(std::shared_ptr<DocumentSessionImpl> session);

		loaders::LazyLoaderWithInclude include(std::string path)
		{
			auto lazy_loader_with_include = loaders::LazyMultiLoaderWithInclude::create(_session);
			return lazy_loader_with_include.include(path);
		}

		template<typename TResult>
		Lazy<DocumentsByIdsMap<TResult>> load_starting_with(std::string id_prefix,
			std::optional<std::string> matches = {},
			int32_t start = 0,
			int32_t page_size = 25,
			std::optional<std::string> exclude = {},
			std::optional<std::string> start_after = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			auto lazy_start_with_operation = std::make_shared<LazyStartsWithOperation<TResult>>(_session,
				std::move(id_prefix),
				std::move(matches),
				start,
				page_size,
				std::move(exclude),
				std::move(start_after),
				from_json, to_json);

			auto get_operation_result = [=](std::shared_ptr<operations::lazy::ILazyOperation> operation)->DocumentsByIdsMap<TResult>
			{
				auto lazy_start_with_op = std::static_pointer_cast<LazyStartsWithOperation<TResult>>(operation);
				return lazy_start_with_op->get_result();				
			};

			return _session->add_lazy_operation<DocumentsByIdsMap<TResult>>(lazy_start_with_operation,
				get_operation_result, [](){});
		}

		template<typename TResult>
		Lazy<std::shared_ptr<TResult>> load(const std::string& id,
			std::optional<std::function<void(const std::shared_ptr<TResult>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			if(_session->is_loaded(id))
			{
				return Lazy<std::shared_ptr<TResult>>([=, *this]()
				{
					return _session->load<TResult>(id, from_json, to_json);
				});
			}

			auto load_operation = std::make_unique<LoadOperation>(_session);
			load_operation->by_id(id);
			auto lazy_load_operation = std::make_shared<LazyLoadOperation<TResult>>(_session, std::move(load_operation),
				from_json, to_json);
			lazy_load_operation->by_id(id);

			auto get_operation_result = [=](std::shared_ptr<operations::lazy::ILazyOperation> operation)->std::shared_ptr<TResult>
			{
				auto lazy_load_op = std::static_pointer_cast<LazyLoadOperation<TResult>>(operation);
				auto&& res = lazy_load_op->get_result();
				if (auto it = res.find(id);
					it == res.end())
				{
					return {};
				}
				else
				{
					return it->second;
				}
			};

			return _session->add_lazy_operation<std::shared_ptr<TResult>>(lazy_load_operation,
				get_operation_result, [=]()
			{
				if (on_eval)
					(*on_eval)(get_operation_result(std::static_pointer_cast<operations::lazy::ILazyOperation>(lazy_load_operation)));
			});
		}

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		Lazy<DocumentsByIdsMap<T>> load(InputIt ids_first, InputIt ids_last,
			std::optional<std::function<void(const DocumentsByIdsMap<T>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>, 
				"'InputIt' - invalid iterator type");

			std::vector<std::reference_wrapper<const std::string>> ids{};
			std::transform(ids_first, ids_last, std::back_inserter(ids), [](const std::string& id)
			{
				return std::cref(id);
			});
			return _session->lazy_load_internal<T>(ids, {}, on_eval, from_json, to_json);
		}
	};
}
