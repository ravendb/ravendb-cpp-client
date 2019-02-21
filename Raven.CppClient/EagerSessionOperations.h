#pragma once

namespace ravendb::client::documents::session
{
	class AdvancedSessionOperations;
	class DocumentSessionImpl;
	struct ResponseTimeInformation;
}

namespace ravendb::client::documents::session::operations::lazy
{
	class EagerSessionOperations
	{
	public:
		friend class AdvancedSessionOperations;

	private:
		std::shared_ptr<DocumentSessionImpl> _session_impl;

		explicit EagerSessionOperations(std::shared_ptr<DocumentSessionImpl> session_impl);

	public:
		~EagerSessionOperations() = default;

		ResponseTimeInformation execute_all_pending_lazy_operations();
	};
}