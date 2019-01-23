#include "stdafx.h"
#include "DocumentSessionImpl.h"
#include "BatchOperation.h"
#include "SessionOptions.h"

namespace ravendb::client::documents::session
{
	DocumentSessionImpl::DocumentSessionImpl(DocumentStoreBase& document_store, SessionOptions options)
		: InMemoryDocumentSessionOperations(document_store, std::move(options))
	{}

	operations::LoadOperation DocumentSessionImpl::load_impl(const std::string& id)
	{
		if (impl::utils::is_blank(id))
		{
			throw std::invalid_argument("Id must be a non empty string");
		}

		auto load_op = operations::LoadOperation(*this);
		load_op.by_id(id);
		auto cmd = load_op.create_request();

		if (cmd)
		{
			_request_executor->execute(*cmd);
			load_op.set_result(cmd->get_result());
		}

		return load_op;
	}


	bool DocumentSessionImpl::exists(const std::string& id)
	{
		if (_known_missing_ids.find(id) != _known_missing_ids.end())
		{
			return false;
		}
		if (_documents_by_id.find(id) != _documents_by_id.end())
		{
			return true;
		}

		//TODO execute HeadDocumentCommand , for now->
		throw std::runtime_error("Not implemented");
	}

	void DocumentSessionImpl::save_changes()
	{
		auto save_changes_operation = operations::BatchOperation(*this);
		if (auto command = save_changes_operation.create_request();
			!command)
		{
			return;
		}
		else
		{
			if (no_tracking)
			{
				throw std::runtime_error("Cannot execute save_changes() when entity tracking is disabled in session.");
			}
			_request_executor->execute(*command);
			update_session_after_changes(command->get_result());
			save_changes_operation.set_result(command->get_result());
		}
	}

}
