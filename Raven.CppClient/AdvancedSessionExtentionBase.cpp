#include "stdafx.h"
#include "AdvancedSessionExtentionBase.h"

namespace ravendb::client::documents::session
{
	AdvancedSessionExtentionBase::AdvancedSessionExtentionBase(
		std::shared_ptr<InMemoryDocumentSessionOperations> session_param)
		: session(session_param)
		, documents_by_entity(session->_documents_by_entity)
		, request_executor(session->get_request_executor())
		, session_info(session->_session_info)
		, document_store(session->get_document_store())
		, deferred_commands_map(session->_deferred_commands_map)
		, deleted_entities(session->_deleted_entities)
		, documents_by_id(session->_documents_by_id)
	{}

	void AdvancedSessionExtentionBase::defer(
		const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
	{
		session->defer(commands);
	}
}
