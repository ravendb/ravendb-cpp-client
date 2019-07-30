#pragma once
#include "EventArgs.h"
#include "MetadataAsDictionary.h"

namespace ravendb::client::documents::session
{
	class InMemoryDocumentSessionOperations;

	class AfterSaveChangesEventArgs : public primitives::EventArgs
	{
	private:
		mutable std::shared_ptr<json::MetadataAsDictionary> _document_metadata{};

		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;
		const std::string _document_id;
		std::shared_ptr<void> _entity{};

	public:
		AfterSaveChangesEventArgs(std::shared_ptr<InMemoryDocumentSessionOperations> _session,
			std::string document_id,
			std::shared_ptr<void> entity);

		std::shared_ptr<InMemoryDocumentSessionOperations> get_session() const;

		const std::string& get_document_id() const;

		std::shared_ptr<void> get_entity() const;

		std::shared_ptr<json::MetadataAsDictionary> get_document_metadata() const;

	};
}
