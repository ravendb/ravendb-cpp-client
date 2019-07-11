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

	std::shared_ptr<IAttachmentsSessionOperations> AdvancedSessionOperations::attachments()
	{
		return _session_impl->attachments();
	}

	std::shared_ptr<IDocumentStore> AdvancedSessionOperations::get_document_store() const
	{
		return _session_impl->get_document_store();
	}

	std::unordered_map<std::string, std::shared_ptr<void>>& AdvancedSessionOperations::get_external_state() const
	{
		return _session_impl->external_state;
	}

	std::shared_ptr<const http::ServerNode> AdvancedSessionOperations::get_current_session_node() const
	{
		return _session_impl->get_current_session_node();
	}

	std::shared_ptr<http::RequestExecutor> AdvancedSessionOperations::get_request_executor() const
	{
		return _session_impl->get_request_executor();
	}

	void AdvancedSessionOperations::add_before_store_listener(primitives::EventHandler handler)
	{
		_session_impl->add_before_store_listener(std::move(handler));
	}

	void AdvancedSessionOperations::remove_before_store_listener(const primitives::EventHandler& handler)
	{
		_session_impl->remove_before_store_listener(handler);
	}

	void AdvancedSessionOperations::add_after_save_changes_listener(primitives::EventHandler handler)
	{
		_session_impl->add_after_save_changes_listener(std::move(handler));
	}

	void AdvancedSessionOperations::remove_after_save_changes_listener(const primitives::EventHandler& handler)
	{
		_session_impl->remove_after_save_changes_listener(handler);
	}

	void AdvancedSessionOperations::add_before_delete_listener(primitives::EventHandler handler)
	{
		_session_impl->add_before_delete_listener(std::move(handler));
	}

	void AdvancedSessionOperations::remove_before_delete_listener(const primitives::EventHandler& handler)
	{
		_session_impl->remove_before_delete_listener(handler);
	}

	void AdvancedSessionOperations::add_before_query_listener(primitives::EventHandler handler)
	{
		_session_impl->add_before_query_listener(std::move(handler));
	}

	void AdvancedSessionOperations::remove_before_query_listener(const primitives::EventHandler& handler)
	{
		_session_impl->remove_before_query_listener(handler);
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

	void AdvancedSessionOperations::wait_for_indexes_after_save_changes(std::function<void(
		InMemoryDocumentSessionOperations::IndexesWaitOptsBuilder&)> options)
	{
		_session_impl->wait_for_indexes_after_save_changes(options);
	}

	void AdvancedSessionOperations::set_transaction_mode(TransactionMode mode)
	{
		_session_impl->set_transaction_mode(mode);
	}

	const EntityToJson& AdvancedSessionOperations::get_entity_to_json() const
	{
		return _session_impl->get_entity_to_json();
	}

	bool AdvancedSessionOperations::exists(std::string id)
	{
		return _session_impl->exists(std::move(id));
	}
}
