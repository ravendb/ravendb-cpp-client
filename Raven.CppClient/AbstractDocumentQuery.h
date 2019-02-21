#pragma once
#include <unordered_set>
#include <ostream>
#include "QueryOperator.h"
#include "IndexQuery.h"
#include "QueryResult.h"
#include "QueryOperation.h"

namespace ravendb::client::documents
{
	namespace session
	{
		class DocumentSessionImpl;
		class InMemoryDocumentSessionOperations;
	}
	namespace queries
	{
		struct IndexQuery;
	}
}
namespace ravendb::client::documents::session
{
	class AbstractDocumentQuery
	{
	private:
		static std::chrono::milliseconds default_timeout;

		std::optional<std::string> index_name{};

		std::optional<std::string> collection_name {};

		DocumentConventions _conventions;

	protected:
		queries::QueryOperator default_operator = queries::QueryOperator::AND;

		Parameters query_parameters{};

		std::reference_wrapper<InMemoryDocumentSessionOperations> the_session;

		std::optional<int32_t> page_size{};

		int32_t start{};

		std::optional<tokens::FieldsToFetchToken> fields_to_fetch_token{};

		std::chrono::milliseconds timeout = default_timeout;

		bool the_wait_for_non_stale_results{};

		std::unordered_set<std::string> document_includes{};
		
		bool disable_entities_tracking{};

		bool disable_caching{};

		std::optional<operations::QueryOperation> query_operation{};
		
		std::optional<std::string> query_raw{};

	public:
		const std::optional<std::string>& get_index_name() const;

		const std::optional<std::string>& get_collection_name() const;

		const DocumentConventions& get_conventions() const;

		std::reference_wrapper<DocumentSessionImpl> get_session() const;

		const std::optional<operations::QueryOperation>& get_query_operation() const;

		template<typename T>
		std::vector<std::shared_ptr<T>> to_list();

		int32_t count();

		queries::QueryResult get_query_result();

	protected:

		void _using_default_operator(queries::QueryOperator query_operator);

		queries::IndexQuery get_index_query() const;

		void _add_parameter(std::string name, nlohmann::json object);

		void _take(int32_t count);

		void _skip(int32_t count);

		void _wait_for_non_stale_results(std::optional<std::chrono::milliseconds> wait_timeout);

		friend std::ostream& operator<<(std::ostream& os, const AbstractDocumentQuery& adq);

		void _no_tracking();

		void _no_caching();

		//TODO see what to send by value/reference
		AbstractDocumentQuery(InMemoryDocumentSessionOperations& session, std::optional<std::string> index_name,
			std::optional<std::string> collection_name, bool is_group_by, /*DeclareToken*/ void* declare_token,
			/*std::vector<DeclareToken>*/void* load_tokens, std::optional<std::string> from_alias);

		operations::QueryOperation initialize_query_operation() const;
		

		queries::IndexQuery generate_index_query(std::string query) const;

		void init_sync();

	private:
		void assert_no_raw_query() const;

		void execute_actual_query();

		template<typename T>
		std::vector<std::shared_ptr<T>> execute_query_operation(std::optional<int32_t> take);

	};

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery::to_list()
	{
		return execute_query_operation<T>({});
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> AbstractDocumentQuery::execute_query_operation(std::optional<int32_t> take)
	{
		if (take && (page_size || *page_size > *take))
		{
			_take(*take);
		}
		init_sync();

		return query_operation->complete<T>();
	}
}
