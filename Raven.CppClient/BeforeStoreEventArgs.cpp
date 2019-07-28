#include "stdafx.h"
#include "InMemoryDocumentSessionOperations.h"
#include "BeforeStoreEventArgs.h"

namespace ravendb::client::documents::session
{
	BeforeStoreEventArgs::BeforeStoreEventArgs(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::string document_id, std::shared_ptr<void> entity)
		: _session(session)
		, _document_id(std::move(document_id))
		, _entity(entity)
	{}

	std::shared_ptr<InMemoryDocumentSessionOperations> BeforeStoreEventArgs::get_session() const
	{
		return _session.lock();
	}

	const std::string& BeforeStoreEventArgs::get_document_id() const
	{
		return _document_id;
	}

	std::shared_ptr<void> BeforeStoreEventArgs::get_entity() const
	{
		return _entity;
	}

	bool BeforeStoreEventArgs::is_metadata_accesses() const
	{
		return _document_metadata.operator bool();
	}

	std::shared_ptr<json::MetadataAsDictionary> BeforeStoreEventArgs::get_document_metadata() const
	{
		if (!_document_metadata)
		{
			_document_metadata = _session.lock()->get_metadata_for(_entity);
		}
		return _document_metadata;
	}
}
