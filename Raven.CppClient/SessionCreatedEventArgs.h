#pragma once
#include "EventArgs.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::primitives
{
	class SessionCreatedEventArgs : public EventArgs
	{
	private:
		const std::weak_ptr<documents::session::InMemoryDocumentSessionOperations> session;

	public:
		explicit SessionCreatedEventArgs(std::shared_ptr<documents::session::InMemoryDocumentSessionOperations> session_param)
			: session(session_param)
		{}

		std::shared_ptr<documents::session::InMemoryDocumentSessionOperations> get_session() const
		{
			return session.lock();
		}
	};
}
