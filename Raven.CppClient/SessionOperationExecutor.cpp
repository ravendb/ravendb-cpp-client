#include "stdafx.h"
#include "SessionOperationExecutor.h"
#include "InMemoryDocumentSessionOperations.h"

ravendb::client::documents::operations::SessionOperationExecutor::SessionOperationExecutor(
	std::shared_ptr<session::InMemoryDocumentSessionOperations> session)
	: OperationExecutor(session->get_document_store())
	, _session(session)
{}
