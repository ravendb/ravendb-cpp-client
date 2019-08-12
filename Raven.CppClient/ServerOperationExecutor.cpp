#include "stdafx.h"
#include "ServerOperationExecutor.h"
#include "ServerWideOperation.h"

namespace ravendb::client::serverwide::operations
{
	ServerOperationExecutor::ServerOperationExecutor(std::shared_ptr<documents::DocumentStore> store)
		: _request_executor(store->get_conventions()->is_disable_topology_updates() ?
			http::ClusterRequestExecutor::create_for_single_node(store->get_urls().at(0), store->get_certificate_details(),
				store->get_conventions(), store->get_scheduler(),
				store->get_before_perform(), store->get_after_perform()) : 
			http::ClusterRequestExecutor::create(store->get_urls(), store->get_certificate_details(),
				store->get_scheduler(), store->get_conventions(),
				store->get_before_perform(), store->get_after_perform()))
	{
		store->add_after_close_listener(std::function<void(const documents::DocumentStore*, const primitives::EventArgs&)>(
			[re = _request_executor](const documents::DocumentStore*, const primitives::EventArgs&)
		{
			re->close();
		}));
	}

	ServerOperationExecutor::~ServerOperationExecutor()
	{
		try
		{
			close();
		}
		catch (...)
		{}
	}

	std::shared_ptr<ServerOperationExecutor> ServerOperationExecutor::create(
		std::shared_ptr<documents::DocumentStore> store)
	{
		auto object = std::shared_ptr<ServerOperationExecutor>(new ServerOperationExecutor(store));
		object->_weak_this = object;
		return object;
	}

	void ServerOperationExecutor::send(const IVoidServerOperation& operation)
	{
		auto command = operation.get_command(_request_executor->get_conventions());
		_request_executor->execute(*command);
	}

	std::unique_ptr<documents::operations::Operation> ServerOperationExecutor::send_async(
		const IServerOperation<documents::operations::OperationIdResult>& operation)
	{
		auto command = operation.get_command(_request_executor->get_conventions());
		_request_executor->execute(*command);

		return std::make_unique<ServerWideOperation>(_request_executor, _request_executor->get_conventions(),
			command->get_result()->operation_id);
	}

	void ServerOperationExecutor::close()
	{
		_request_executor->close();
	}
}
