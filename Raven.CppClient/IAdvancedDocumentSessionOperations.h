#pragma once
#include "IDocumentStore.h"
#include "RequestExecutor.h"
#include "DateTimeOffset.h"
#include "TransactionMode.h"

namespace ravendb::client::documents::session
{
	struct IAdvancedDocumentSessionOperations
	{
		virtual ~IAdvancedDocumentSessionOperations() = 0;

		virtual std::reference_wrapper<IDocumentStore> get_document_store() const = 0;

		virtual std::shared_ptr<http::RequestExecutor> get_request_executor() const = 0;

		virtual bool has_changes() const = 0;

		virtual int32_t get_max_number_of_requests_per_session() const = 0;

		virtual void set_max_number_of_requests_per_session(int32_t max_requests) = 0;

		virtual int32_t get_number_of_requests() const = 0;

		virtual bool is_use_optimistic_concurrency() const = 0;

		virtual bool set_use_optimistic_concurrency() = 0;

		virtual void clear() = 0;

		//can't use template in virtual method
		virtual void evict(std::shared_ptr<void> entity) = 0;

		virtual std::string get_document_id(std::shared_ptr<void> entity) = 0;

		virtual std::string get_change_vector(std::shared_ptr<void> entity) = 0;

		virtual impl::DateTimeOffset get_last_modified(std::shared_ptr<void> entity) = 0;

		virtual bool has_changed(std::shared_ptr<void> entity) = 0;

		virtual bool is_loaded(const std::string& id) = 0;

		virtual void ignore_changes(std::shared_ptr<void> entity) = 0;

		virtual void set_transaction_mode(TransactionMode mode) = 0;
	};

	inline IAdvancedDocumentSessionOperations::~IAdvancedDocumentSessionOperations() = default;
}
