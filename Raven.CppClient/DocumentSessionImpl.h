#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "DocumentsByIdsMap.h"
#include "LoadOperation.h"
#include "RawDocumentQuery.h"
#include "JavaScriptArray.h"
#include "ResponseTimeInformation.h"
#include "Lazy.h"
#include "LazyLoadOperation.h"
#include "DocumentQuery.h"
#include "Query.h"

namespace ravendb::client::documents
{
	namespace session::loaders
	{
		class LoaderWithInclude;
	}
	namespace operations
	{
		struct PatchRequest;
	}

	namespace session::operations::lazy
	{
		class LazySessionOperations;
	}
}

namespace ravendb::client::documents::session
{
	class DocumentSessionImpl : public InMemoryDocumentSessionOperations
	{
	private:
		int32_t _vals_count{};
		int32_t _custom_count{};

		operations::LoadOperation load_impl(const std::string& id);

		operations::LoadOperation load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids);

		operations::LoadOperation load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes);

		void load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
			operations::LoadOperation& operation);

		void patch_internal(const std::string& id, const std::string& path, const nlohmann::json& value,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);

		void patch_internal(const std::string& id, const std::string& script,
			const std::unordered_map<std::string, nlohmann::json>& values,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);

		void increment_internal(const std::string& id, const std::string& path, const nlohmann::json& value_to_add,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);

		bool try_merge_patches(const std::string& id, const documents::operations::PatchRequest& patch_request);

		//return value = should retry
		bool execute_lazy_operations_single_step(ResponseTimeInformation& response_time_information,
			const std::vector<commands::multi_get::GetRequest>& requests);

		DocumentSessionImpl(std::shared_ptr<DocumentStoreBase> document_store,/* UUID id,*/ SessionOptions options);

	protected:
		std::string generate_id(std::type_index type, std::shared_ptr<void> entity) const override;

	public:
		~DocumentSessionImpl() override;

		static std::shared_ptr<DocumentSessionImpl> create(std::shared_ptr<DocumentStoreBase> document_store,/* UUID id,*/
			SessionOptions options);

		operations::lazy::LazySessionOperations lazily();

		loaders::LoaderWithInclude include(const std::string& path);

		//TODO add custom (de)serializers to/from json
		template<typename T>
		Lazy<T> add_lazy_operation(std::shared_ptr<operations::lazy::ILazyOperation> operation,
			std::function<T(std::shared_ptr<operations::lazy::ILazyOperation>)> get_operation_result,
			std::function<void()> on_eval);

		Lazy<int32_t> add_lazy_count_operation(std::shared_ptr<operations::lazy::ILazyOperation> operation);

		ResponseTimeInformation execute_all_pending_lazy_operations();

		template<typename T>
		Lazy<DocumentsByIdsMap<T>> lazy_load_internal(
			const std::vector<std::reference_wrapper<const std::string>>& ref_ids,
			const std::vector<std::string>& includes,
			std::optional<std::function<void(const DocumentsByIdsMap<T>&)>> on_eval = {},
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		template <typename T>
		DocumentsByIdsMap<T> load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
			const std::vector<std::string>& includes,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json,
			const std::optional<DocumentInfo::ToJsonConverter>& to_json);

		template<typename T>
		std::shared_ptr<T> load(const std::string& id, 
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		//load by collections of ids (std::string)
		template<typename T, typename InputIt>
		DocumentsByIdsMap<T> load(InputIt first, InputIt last,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {});

		bool exists(const std::string& id);

		void save_changes();

		template<typename T>
		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> raw_query(std::string query);

		template<typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> query();

		template<typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> query(const queries::Query& collection_or_index_name);

		template<typename T, typename TIndex>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> query();

		template <typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> document_query(
			std::optional<std::string> index_name,
			std::optional<std::string> collection_name,
			bool is_map_reduced);

		template <typename T>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> document_query();

		template<typename T, typename TIndex>
		std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> document_query();

		template<typename T>
		void patch(const std::string& id, const std::string& path_to_array,
			std::function<void(JavaScriptArray<T>&)> array_adder,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);

		template<typename T>
		void patch(const std::string& id, const std::string& path, const T& value,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);

		template<typename T>
		void increment(const std::string& id, const std::string& path, const T& value_to_add,
			const std::optional<DocumentInfo::EntityUpdater>& update_from_json);
	};

	template <typename T>
	Lazy<T> DocumentSessionImpl::add_lazy_operation(std::shared_ptr<operations::lazy::ILazyOperation> operation,
		std::function<T(std::shared_ptr<operations::lazy::ILazyOperation>)> get_operation_result,
		 std::function<void()> on_eval)
	{
		if (!get_operation_result)
		{
			throw std::invalid_argument("'get_operation_result' should have a target");
		}

		_pending_lazy_operations.push_back(operation);

		auto lazy_value = Lazy<T>([this,get_operation_result,operation]()
		{
			execute_all_pending_lazy_operations();
			return get_operation_result(operation);
		});

		_on_evaluate_lazy.insert_or_assign(operation, on_eval);

		return std::move(lazy_value);
	}

	template<typename T>
	Lazy<DocumentsByIdsMap<T>> DocumentSessionImpl::lazy_load_internal(
		const std::vector<std::reference_wrapper<const std::string>>& ref_ids,
		const std::vector<std::string>& includes,
		std::optional<std::function<void(const DocumentsByIdsMap<T>&)>> on_eval,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		std::vector<std::string> ids{};
		std::transform(ref_ids.begin(), ref_ids.end(), std::back_inserter(ids), []
		(const std::reference_wrapper<const std::string>& id)
		{
			return id.get();
		});

		if (check_if_already_included(ids, includes))
		{
			return Lazy<DocumentsByIdsMap<T>>([this, ids = ids, from_json, to_json]()
			{
				return load<T>(ids.begin(), ids.end(), from_json, to_json);
			});
		}

		auto load_operation = std::make_unique<operations::LoadOperation>(_weak_this.lock());
		load_operation->by_ids(ref_ids).with_includes(includes);

		auto lazy_load_operation = std::make_shared<operations::lazy::LazyLoadOperation<T>>(_weak_this.lock(), std::move(load_operation));
		lazy_load_operation->by_ids(ids).with_includes(includes);

		auto get_operation_result = [](std::shared_ptr<operations::lazy::ILazyOperation> operation)->DocumentsByIdsMap<T>
		{
			auto lazy_load_op = std::static_pointer_cast<operations::lazy::LazyLoadOperation<T>>(operation);

			return lazy_load_op->get_result();
		};

		return this->add_lazy_operation<DocumentsByIdsMap<T>>(lazy_load_operation,
			get_operation_result, [=]()
		{
			if (on_eval)
				(*on_eval)(get_operation_result(std::static_pointer_cast<operations::lazy::ILazyOperation>(lazy_load_operation)));
		});
	}

	template <typename T>
	DocumentsByIdsMap<T> DocumentSessionImpl::load_internal(
		const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return load_impl(ids, includes).get_documents<T>(from_json, to_json);
	}

	template <typename T>
	std::shared_ptr<T> DocumentSessionImpl::load(const std::string& id,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		return id.empty() ? nullptr : load_impl(id).get_document<T>(from_json, to_json);
	}

	template <typename T, typename InputIt>
	DocumentsByIdsMap<T> DocumentSessionImpl::load(InputIt first, InputIt last,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		const std::optional<DocumentInfo::ToJsonConverter>& to_json)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, std::string>, "invalid iterator type");

		std::vector<std::reference_wrapper<const std::string>> ids{};
		std::transform(first, last, std::back_inserter(ids), [](const std::string& id) {return std::cref(id); });

		return load_impl(ids).get_documents<T>(from_json, to_json);
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> DocumentSessionImpl::raw_query(std::string query)
	{
		return RawDocumentQuery<T>::create(_weak_this.lock(), std::move(query));
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::query()
	{
		return document_query<T>({}, {}, false);
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::query(
		const queries::Query& collection_or_index_name)
	{
		if(collection_or_index_name.get_collection() && !collection_or_index_name.get_collection()->empty())
		{
			return document_query<T>({}, collection_or_index_name.get_collection(), false);
		}

		return document_query<T>(collection_or_index_name.get_index_name(), {}, false);
	}

	template <typename T, typename TIndex>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::query()
	{
		return document_query<T, TIndex>();
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::document_query()
	{
		return document_query<T>({}, {}, false);
	}

	template <typename T, typename TIndex>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::document_query()
	{
		static_assert(std::is_base_of_v<indexes::AbstractIndexCreationTask, TIndex>, "'TIndex' must be derived from AbstractIndexCreationTask");

		try
		{
			auto index = TIndex();
			return document_query<T>(index.get_index_name(), {}, index.is_map_reduce());
		}
		catch (std::exception& e)
		{
			std::ostringstream msg{};
			msg << "Unable to query index : " << impl::utils::GetCppClassName::get_simple_class_name(typeid(TIndex))
				<< " " << e.what();
			throw std::runtime_error(msg.str());
		}
	}

	template <typename T>
	std::shared_ptr<IDocumentQuery<T, DocumentQuery<T>>> DocumentSessionImpl::document_query(
		std::optional<std::string> index_name,
		std::optional<std::string> collection_name,
		bool is_map_reduced)
	{
		process_query_parameters<T>(index_name, collection_name, get_conventions());

		return DocumentQuery<T>::create(_weak_this.lock(), index_name, collection_name, is_map_reduced);
	}

	template <typename T>
	void DocumentSessionImpl::patch(const std::string& id, const std::string& path, const T& value,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{	
		patch_internal(id, path, nlohmann::json(value), update_from_json);
	}

	template <typename T>
	void DocumentSessionImpl::patch(const std::string& id, const std::string& path_to_array,
		std::function<void(JavaScriptArray<T>&)> array_adder,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{
		auto script_array = JavaScriptArray<T>(_custom_count++, path_to_array);
		array_adder(script_array);

		patch_internal(id, script_array.get_script(), script_array.get_parameters(), update_from_json);
	}


	template <typename T>
	void DocumentSessionImpl::increment(const std::string& id, const std::string& path, const T& value_to_add,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{
		increment_internal(id, path, nlohmann::json(value_to_add), update_from_json);
	}
}
