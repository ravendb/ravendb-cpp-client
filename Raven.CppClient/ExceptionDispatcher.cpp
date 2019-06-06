#include "stdafx.h"
#include "ExceptionDispatcher.h"
#include "HttpStatus.h"
#include "DocumentConflictException.h"
#include "ConcurrencyException.h"
#include "IndexCompilationException.h"
#include "TimeoutException.h"
#include "AllTopologyNodesDownException.h"
#include "BadRequestException.h"
#include "BadResponseException.h"
#include "ClientVersionMismatchException.h"
#include "InvalidQueryException.h"
#include "UnsuccessfulRequestException.h"
#include "AttachmentDoesNotExistException.h"
#include "ChangeProcessingException.h"
#include "CommandExecutionException.h"
#include "NodeIsPassiveException.h"
#include "NoLeaderException.h"
#include "LicenseActivationException.h"
#include "DatabaseConcurrentLoadTimeoutException.h"
#include "DatabaseDisabledException.h"
#include "DatabaseDoesNotExistException.h"
#include "DatabaseLoadFailureException.h"
#include "DatabaseLoadTimeoutException.h"
#include "DatabaseNotRelevantException.h"
#include "DatabaseSchemaErrorException.h"
#include "DocumentDoesNotExistException.h"
#include "BulkInsertAbortedException.h"
#include "BulkInsertProtocolViolationException.h"
#include "CounterOverflowException.h"
#include "IndexAlreadyExistException.h"
#include "IndexCreationException.h"
#include "IndexDeletionException.h"
#include "IndexInvalidException.h"
#include "IndexDoesNotExistException.h"
#include "JavaScriptException.h"
#include "JavaScriptParseException.h"
#include "RevisionsDisabledException.h"
#include "NonUniqueObjectException.h"
#include "SubscriberErrorException.h"
#include "SubscriptionChangeVectorUpdateConcurrencyException.h"
#include "SubscriptionClosedException.h"
#include "SubscriptionDoesNotBelongToNodeException.h"
#include "SubscriptionDoesNotExistException.h"
#include "SubscriptionInUseException.h"
#include "SubscriptionInvalidStateException.h"
#include "RouteNotFoundException.h"
#include "AuthenticationException.h"
#include "AuthorizationException.h"
#include "CertificateNameMismatchException.h"
#include "ServerLoadFailureException.h"

namespace ravendb::client::exceptions
{
	void from_json(const nlohmann::json& j, ExceptionSchema& es)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Url", es.url);
		get_val_from_json(j, "Type", es.type);
		get_val_from_json(j, "Message", es.message);
		get_val_from_json(j, "Error", es.error);
	}

	void ExceptionDispatcher::throw_conflict(const ExceptionSchema& schema, const std::string& json)
	{
		if(schema.type.find("DocumentConflictException") != std::string::npos)
		{
			throw documents::DocumentConflictException::from_json(json);
		}
		throw ConcurrencyException(schema.error);
	}

	static const auto make_exception_with_inner = [](auto exception, std::exception_ptr inner)->std::exception_ptr
	{
		try
		{
			try
			{
				std::rethrow_exception(inner);
			}
			catch (...)
			{
				std::throw_with_nested(std::move(exception));
			}
		}
		catch (...)
		{
			return std::current_exception();
		}
	};

	static const auto make_exception_without_inner = [](auto exception)->std::exception_ptr
	{
		return std::make_exception_ptr(std::move(exception));

	};

	static const auto make_exception = [](auto exception, std::exception_ptr inner)->std::exception_ptr
	{
		if (inner)
		{
			return make_exception_with_inner(std::move(exception), inner);
		}
		return make_exception_without_inner(std::move(exception));
	};

	std::exception_ptr ExceptionDispatcher::get_raven_exception_from(const ExceptionSchema& schema, const std::string& message,
		std::exception_ptr inner)
	{
		const auto& type_as_string = schema.type;

		if(type_as_string.empty())
		{
			return nullptr;
		}

		const auto type_contains = [&](const std::string& type_name)-> bool
		{
			return type_as_string.find(type_name) != std::string::npos;
		};

		if(type_contains("System.TimeoutException"))
		{
			return make_exception(TimeoutException(message), inner);
		}

		if (type_contains("RavenException"))
		{
			return make_exception(RavenException(message), inner);
		}
		if (type_contains("AllTopologyNodesDownException"))
		{
			return make_exception(AllTopologyNodesDownException(message), inner);
		}
		if (type_contains("BadRequestException"))
		{
			return make_exception(BadRequestException(message), inner);
		}
		if (type_contains("BadResponseException"))
		{
			return make_exception(BadResponseException(message), inner);
		}
		if (type_contains("ClientVersionMismatchException"))
		{
			return make_exception(ClientVersionMismatchException(message), inner);
		}
		if (type_contains("ConcurrencyException"))
		{
			return make_exception(ConcurrencyException(message), inner);
		}
		if (type_contains("InvalidQueryException"))
		{
			return make_exception(InvalidQueryException(message), inner);
		}
		if (type_contains("TimeoutException"))
		{
			return make_exception(TimeoutException(message), inner);
		}
		if (type_contains("UnsuccessfulRequestException"))
		{
			return make_exception(UnsuccessfulRequestException(message), inner);
		}
		////
		if (type_contains("AttachmentDoesNotExistException"))
		{
			return make_exception(attachments::AttachmentDoesNotExistException(message), inner);
		}
		////
		if (type_contains("ChangeProcessingException"))
		{
			return make_exception(changes::ChangeProcessingException(message), inner);
		}
		////
		if (type_contains("CommandExecutionException"))
		{
			return make_exception(cluster::CommandExecutionException(message), inner);
		}
		if (type_contains("NodeIsPassiveException"))
		{
			return make_exception(cluster::NodeIsPassiveException(message), inner);
		}
		if (type_contains("NoLeaderException"))
		{
			return make_exception(cluster::NoLeaderException(message), inner);
		}
		////
		if (type_contains("LicenseActivationException"))
		{
			return make_exception(commercial::LicenseActivationException(message), inner);
		}
		////
		if (type_contains("CompilationException"))
		{
			return make_exception(compilation::CompilationException(message), inner);
		}
		////
		if (type_contains("DatabaseConcurrentLoadTimeoutException"))
		{
			return make_exception(database::DatabaseConcurrentLoadTimeoutException(message), inner);
		}
		if (type_contains("DatabaseDisabledException"))
		{
			return make_exception(database::DatabaseDisabledException(message), inner);
		}
		if (type_contains("DatabaseDoesNotExistException"))
		{
			return make_exception(database::DatabaseDoesNotExistException(message), inner);
		}
		if (type_contains("DatabaseLoadFailureException"))
		{
			return make_exception(database::DatabaseLoadFailureException(message), inner);
		}
		if (type_contains("DatabaseLoadTimeoutException"))
		{
			return make_exception(database::DatabaseLoadTimeoutException(message), inner);
		}
		if (type_contains("DatabaseNotRelevantException"))
		{
			return make_exception(database::DatabaseNotRelevantException(message), inner);
		}
		if (type_contains("DatabaseSchemaErrorException"))
		{
			return make_exception(database::DatabaseSchemaErrorException(message), inner);
		}
		////
		if (type_contains("DocumentConflictException"))
		{
			return make_exception(documents::DocumentConflictException::from_message(message), inner);
		}
		if (type_contains("DocumentDoesNotExistException"))
		{
			return make_exception(documents::DocumentDoesNotExistException(message), inner);
		}
		////
		if (type_contains("BulkInsertAbortedException"))
		{
			return make_exception(documents::bulkinsert::BulkInsertAbortedException(message), inner);
		}
		if (type_contains("BulkInsertProtocolViolationException"))
		{
			return make_exception(documents::bulkinsert::BulkInsertProtocolViolationException(message), inner);
		}
		////
		if (type_contains("IndexCompilationException"))
		{
			return make_exception(documents::compilation::IndexCompilationException(message), inner);
		}
		////
		if (type_contains("CounterOverflowException"))
		{
			return make_exception(documents::counters::CounterOverflowException(message), inner);
		}
		////
		if (type_contains("IndexAlreadyExistException"))
		{
			return make_exception(documents::indexes::IndexAlreadyExistException(message), inner);
		}
		if (type_contains("IndexCreationException"))
		{
			return make_exception(documents::indexes::IndexCreationException(message), inner);
		}
		if (type_contains("IndexDeletionException"))
		{
			return make_exception(documents::indexes::IndexDeletionException(message), inner);
		}
		if (type_contains("IndexDoesNotExistException"))
		{
			return make_exception(documents::indexes::IndexDoesNotExistException(message), inner);
		}
		if (type_contains("IndexInvalidException"))
		{
			return make_exception(documents::indexes::IndexInvalidException(message), inner);
		}
		////
		if (type_contains("JavaScriptException"))
		{
			return make_exception(documents::patching::JavaScriptException(message), inner);
		}
		if (type_contains("JavaScriptParseException"))
		{
			return make_exception(documents::patching::JavaScriptParseException(message), inner);
		}
		////
		if (type_contains("RevisionsDisabledException"))
		{
			return make_exception(documents::revisions::RevisionsDisabledException(message), inner);
		}
		////
		if (type_contains("NonUniqueObjectException"))
		{
			return make_exception(documents::session::NonUniqueObjectException(message), inner);
		}
		////
		if (type_contains("SubscriberErrorException"))
		{
			return make_exception(documents::subscriptions::SubscriberErrorException(message), inner);
		}
		if (type_contains("SubscriptionChangeVectorUpdateConcurrencyException"))
		{
			return make_exception(documents::subscriptions::SubscriptionChangeVectorUpdateConcurrencyException(message), inner);
		}
		if (type_contains("SubscriptionClosedException"))
		{
			return make_exception(documents::subscriptions::SubscriptionClosedException(message), inner);
		}
		if (type_contains("SubscriptionDoesNotBelongToNodeException"))
		{
			return make_exception(documents::subscriptions::SubscriptionDoesNotBelongToNodeException(message), inner);
		}
		if (type_contains("SubscriptionDoesNotExistException"))
		{
			return make_exception(documents::subscriptions::SubscriptionDoesNotExistException(message), inner);
		}
		if (type_contains("SubscriptionInUseException"))
		{
			return make_exception(documents::subscriptions::SubscriptionInUseException(message), inner);
		}
		if (type_contains("SubscriptionInvalidStateException"))
		{
			return make_exception(documents::subscriptions::SubscriptionInvalidStateException(message), inner);
		}
		////
		if (type_contains("RouteNotFoundException"))
		{
			return make_exception(routing::RouteNotFoundException(message), inner);
		}
		////
		if (type_contains("AuthenticationException"))
		{
			return make_exception(security::AuthenticationException(message), inner);
		}
		if (type_contains("AuthorizationException"))
		{
			return make_exception(security::AuthorizationException(message), inner);
		}
		if (type_contains("CertificateNameMismatchException"))
		{
			return make_exception(security::CertificateNameMismatchException(message), inner);
		}
		if (type_contains("SecurityException"))
		{
			return make_exception(security::SecurityException(message), inner);
		}
		////
		if (type_contains("ServerLoadFailureException"))
		{
			return make_exception(server::ServerLoadFailureException(message), inner);
		}

		return nullptr;
	}

	std::exception_ptr ExceptionDispatcher::get(const ExceptionSchema& schema, int32_t code,
		std::exception_ptr inner)
	{
		const auto& message = schema.message;
		const auto& type_as_string = schema.type;

		if(code == static_cast<int32_t>(http::HttpStatus::SC_CONFLICT))
		{
			if(type_as_string.find("DocumentConflictException") != std::string::npos)
			{
				return std::make_exception_ptr(documents::DocumentConflictException::from_message(message));
			}
			return std::make_exception_ptr(ConcurrencyException(message));
		}

		std::ostringstream error{};
		error << schema.error << "\r\n" << "The server at " << schema.url << " responded with status code: " << code;

		auto exception = get_raven_exception_from(schema, error.str(), inner);

		if(!exception)
		{
			return make_exception(RavenException(error.str()), inner);
		}

		return exception;
	}

	void ExceptionDispatcher::throw_exception(const impl::CurlResponse& response)
	{
		try
		{
			const auto& json = response.output;
			auto json_node = nlohmann::json::parse(json);
			auto schema = json_node.get<ExceptionSchema>();

			if(response.status_code == static_cast<int32_t>(http::HttpStatus::SC_CONFLICT))
			{
				throw_conflict(schema, json);
			}

			auto exception = get_raven_exception_from(schema);

			if(!exception)
			{
				throw RavenException::generic(schema.error, json);
			}

			try
			{
				std::rethrow_exception(exception);
			}
			catch (documents::compilation::IndexCompilationException& index_compilation_exception)
			{
				if (auto it = json_node.find("TransformerDefinitionProperty");
					it != json_node.end())
				{
					index_compilation_exception.index_definition_property = it->get<std::string>();
				}

				if (auto it = json_node.find("ProblematicText");
					it != json_node.end())
				{
					index_compilation_exception.problematic_text = it->get<std::string>();
				}

				throw;
			}
			catch (...)
			{
				throw;
			}
		}
		catch (const RavenException&)
		{
			throw;
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(RavenException(e.what()));
		}
	}
}
