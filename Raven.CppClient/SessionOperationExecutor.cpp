#include "stdafx.h"
#include "SessionOperationExecutor.h"
#include "InMemoryDocumentSessionOperations.h"

ravendb::client::documents::operations::SessionOperationExecutor::SessionOperationExecutor(session::InMemoryDocumentSessionOperations& session)
	: OperationExecutor(session.get_document_store())
	, _session(session)
{}
