#include "stdafx.h"
#include "AdvancedSessionOperations.h"
#include "LazySessionOperations.h"

namespace ravendb::client::documents::session
{
	AdvancedSessionOperations::AdvancedSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl)
		: _session_impl(session_impl)
	{}

	AdvancedSessionOperations::~AdvancedSessionOperations() = default;

	operations::lazy::LazySessionOperations AdvancedSessionOperations::lazily()
	{
		return _session_impl->lazily();
	}

	operations::lazy::EagerSessionOperations AdvancedSessionOperations::eagerly()
	{
		return operations::lazy::EagerSessionOperations(_session_impl);
	}

	std::reference_wrapper<IDocumentStore> AdvancedSessionOperations::get_document_store() const
	{
		return _session_impl->get_document_store();
	}

	std::unordered_map<std::string, std::shared_ptr<void>>& AdvancedSessionOperations::get_external_state() const
	{
		return _session_impl->external_state;
	}

	std::shared_ptr<http::RequestExecutor> AdvancedSessionOperations::get_request_executor() const
	{
		return _session_impl->get_request_executor();
	}

	bool AdvancedSessionOperations::has_changes() const
	{
		return _session_impl->has_changes();
	}

	int32_t AdvancedSessionOperations::get_max_number_of_requests_per_session() const
	{
		return _session_impl->max_number_of_requests_per_session;
	}

	void AdvancedSessionOperations::set_max_number_of_requests_per_session(int32_t max_requests)
	{
		_session_impl->max_number_of_requests_per_session = max_requests;
	}

	int32_t AdvancedSessionOperations::get_number_of_requests() const
	{
		return _session_impl->get_number_of_requests();
	}

	std::string AdvancedSessionOperations::store_identifier() const
	{
		return _session_impl->store_identifier();
	}

	bool AdvancedSessionOperations::is_use_optimistic_concurrency() const
	{
		return _session_impl->use_optimistic_concurrency;
	}

	void AdvancedSessionOperations::set_use_optimistic_concurrency(bool use_optimistic_concurrency)
	{
		_session_impl->use_optimistic_concurrency = use_optimistic_concurrency;
	}

	void AdvancedSessionOperations::clear()
	{
		_session_impl->clear();
	}

	void AdvancedSessionOperations::defer(
		const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
	{
		_session_impl->defer(commands);
	}

	bool AdvancedSessionOperations::is_loaded(const std::string& id) const
	{
		return _session_impl->is_loaded(id);
	}

	std::unordered_map<std::string, std::vector<DocumentsChanges>> AdvancedSessionOperations::what_changed()
	{
		return _session_impl->what_changed();
	}

	void AdvancedSessionOperations::set_transaction_mode(TransactionMode mode)
	{
		_session_impl->set_transaction_mode(mode);
	}

	const EntityToJson& AdvancedSessionOperations::get_entity_to_json() const
	{
		return _session_impl->get_entity_to_json();
	}
}
