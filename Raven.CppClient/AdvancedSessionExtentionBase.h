#pragma once
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::session
{
	class AdvancedSessionExtentionBase
	{
	protected:
		const std::shared_ptr<InMemoryDocumentSessionOperations> session;

		decltype(InMemoryDocumentSessionOperations::_documents_by_entity)& documents_by_entity;
		std::shared_ptr<http::RequestExecutor> request_executor;
		decltype(InMemoryDocumentSessionOperations::_session_info)& session_info;
		std::shared_ptr<IDocumentStore> document_store;
		decltype(InMemoryDocumentSessionOperations::_deferred_commands_map)& deferred_commands_map;

		decltype(InMemoryDocumentSessionOperations::_deleted_entities)& deleted_entities;
		decltype(InMemoryDocumentSessionOperations::_documents_by_id)& documents_by_id;

	protected:
		AdvancedSessionExtentionBase(std::shared_ptr<InMemoryDocumentSessionOperations> session_param);

	public:
		virtual ~AdvancedSessionExtentionBase() = 0;

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands);
	};

	inline AdvancedSessionExtentionBase::~AdvancedSessionExtentionBase() = default;
}
