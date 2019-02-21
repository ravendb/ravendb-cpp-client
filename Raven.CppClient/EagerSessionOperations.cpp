#include "stdafx.h"
#include "EagerSessionOperations.h"
#include "DocumentSessionImpl.h"

namespace ravendb::client::documents::session::operations::lazy
{
	EagerSessionOperations::EagerSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl)
		: _session_impl(session_impl)
	{}

	ResponseTimeInformation EagerSessionOperations::execute_all_pending_lazy_operations()
	{
		return _session_impl->execute_all_pending_lazy_operations();
	}
}
