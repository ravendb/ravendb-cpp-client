#pragma once
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::session
{
	class AdvancedSessionExtensionBase
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
		AdvancedSessionExtensionBase(std::shared_ptr<InMemoryDocumentSessionOperations> session_param);

	public:
		virtual ~AdvancedSessionExtensionBase() = 0;

		void defer(const std::vector<std::shared_ptr<commands::batches::CommandDataBase>>& commands)
		{
			session->defer(commands);
		}
	};

	inline AdvancedSessionExtensionBase::AdvancedSessionExtensionBase(
		std::shared_ptr<InMemoryDocumentSessionOperations> session_param)
		: session(session_param)
		, documents_by_entity(session->_documents_by_entity)
		, request_executor(session->get_request_executor())
		, session_info(session->_session_info)
		, document_store(session->get_document_store())
		, deferred_commands_map(session->_deferred_commands_map)
		, deleted_entities(session->_deleted_entities)
		, documents_by_id(session->_documents_by_id)
	{}

	inline AdvancedSessionExtensionBase::~AdvancedSessionExtensionBase() = default;
}
