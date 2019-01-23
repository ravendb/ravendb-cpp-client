#pragma once
#include "DocumentSessionImpl.h"


namespace ravendb::client::documents::session
{
	class ExtendedAdvancedSessionOperations
	{
	public:
		friend class AdvancedDocumentSessionOperations;

	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

		explicit ExtendedAdvancedSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl)
			: _session_impl(session_impl)
		{}

	public:
	};

}
