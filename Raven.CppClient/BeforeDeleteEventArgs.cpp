#include "stdafx.h"
#include "InMemoryDocumentSessionOperations.h"
#include "BeforeDeleteEventArgs.h"

namespace ravendb::client::documents::session
{
	BeforeDeleteEventArgs::BeforeDeleteEventArgs(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::string document_id, std::shared_ptr<void> entity)
		: _session(session)
		, _document_id(std::move(document_id))
		, _entity(entity)
	{}

	std::shared_ptr<InMemoryDocumentSessionOperations> BeforeDeleteEventArgs::get_session() const
	{
		return _session.lock();
	}

	const std::string& BeforeDeleteEventArgs::get_document_id() const
	{
		return _document_id;
	}

	std::shared_ptr<void> BeforeDeleteEventArgs::get_entity() const
	{
		return _entity;
	}

	std::shared_ptr<json::MetadataAsDictionary> BeforeDeleteEventArgs::get_document_metadata() const
	{
		if (!_document_metadata)
		{
			_document_metadata = _session.lock()->get_metadata_for(_entity);
		}
		return _document_metadata;
	}
}
