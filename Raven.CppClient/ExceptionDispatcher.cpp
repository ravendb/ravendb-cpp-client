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


        std::string errorMessage;
		if(message.empty()) {
            errorMessage = schema.message + ", server stack trace: " + schema.error ;
        }
		else {
            errorMessage = message + ". Server error: " + schema.message  + ", server stack trace: " + schema.error;
        }

		const auto type_contains = [&](const std::string& type_name)-> bool
		{
			return type_as_string.find(type_name) != std::string::npos;
		};

		if(type_contains("System.TimeoutException"))
		{
			return make_exception(TimeoutException(errorMessage), inner);
		}

		if (type_contains("RavenException"))
		{
			return make_exception(RavenException(errorMessage), inner);
		}
		if (type_contains("AllTopologyNodesDownException"))
		{
			return make_exception(AllTopologyNodesDownException(errorMessage), inner);
		}
		if (type_contains("BadRequestException"))
		{
			return make_exception(BadRequestException(errorMessage), inner);
		}
		if (type_contains("BadResponseException"))
		{
			return make_exception(BadResponseException(errorMessage), inner);
		}
		if (type_contains("ClientVersionMismatchException"))
		{
			return make_exception(ClientVersionMismatchException(errorMessage), inner);
		}
		if (type_contains("ConcurrencyException"))
		{
			return make_exception(ConcurrencyException(errorMessage), inner);
		}
		if (type_contains("InvalidQueryException"))
		{
			return make_exception(InvalidQueryException(errorMessage), inner);
		}
		if (type_contains("TimeoutException"))
		{
			return make_exception(TimeoutException(errorMessage), inner);
		}
		if (type_contains("UnsuccessfulRequestException"))
		{
			return make_exception(UnsuccessfulRequestException(errorMessage), inner);
		}
		////
		if (type_contains("AttachmentDoesNotExistException"))
		{
			return make_exception(attachments::AttachmentDoesNotExistException(errorMessage), inner);
		}
		////
		if (type_contains("ChangeProcessingException"))
		{
			return make_exception(changes::ChangeProcessingException(errorMessage), inner);
		}
		////
		if (type_contains("CommandExecutionException"))
		{
			return make_exception(cluster::CommandExecutionException(errorMessage), inner);
		}
		if (type_contains("NodeIsPassiveException"))
		{
			return make_exception(cluster::NodeIsPassiveException(errorMessage), inner);
		}
		if (type_contains("NoLeaderException"))
		{
			return make_exception(cluster::NoLeaderException(errorMessage), inner);
		}
		////
		if (type_contains("LicenseActivationException"))
		{
			return make_exception(commercial::LicenseActivationException(errorMessage), inner);
		}
		////
		if (type_contains("CompilationException"))
		{
			return make_exception(compilation::CompilationException(errorMessage), inner);
		}
		////
		if (type_contains("DatabaseConcurrentLoadTimeoutException"))
		{
			return make_exception(database::DatabaseConcurrentLoadTimeoutException(errorMessage), inner);
		}
		if (type_contains("DatabaseDisabledException"))
		{
			return make_exception(database::DatabaseDisabledException(errorMessage), inner);
		}
		if (type_contains("DatabaseDoesNotExistException"))
		{
			return make_exception(database::DatabaseDoesNotExistException(errorMessage), inner);
		}
		if (type_contains("DatabaseLoadFailureException"))
		{
			return make_exception(database::DatabaseLoadFailureException(errorMessage), inner);
		}
		if (type_contains("DatabaseLoadTimeoutException"))
		{
			return make_exception(database::DatabaseLoadTimeoutException(errorMessage), inner);
		}
		if (type_contains("DatabaseNotRelevantException"))
		{
			return make_exception(database::DatabaseNotRelevantException(errorMessage), inner);
		}
		if (type_contains("DatabaseSchemaErrorException"))
		{
			return make_exception(database::DatabaseSchemaErrorException(errorMessage), inner);
		}
		////
		if (type_contains("DocumentConflictException"))
		{
			return make_exception(documents::DocumentConflictException::from_message(errorMessage), inner);
		}
		if (type_contains("DocumentDoesNotExistException"))
		{
			return make_exception(documents::DocumentDoesNotExistException(errorMessage), inner);
		}
		////
		if (type_contains("BulkInsertAbortedException"))
		{
			return make_exception(documents::bulkinsert::BulkInsertAbortedException(errorMessage), inner);
		}
		if (type_contains("BulkInsertProtocolViolationException"))
		{
			return make_exception(documents::bulkinsert::BulkInsertProtocolViolationException(errorMessage), inner);
		}
		////
		if (type_contains("IndexCompilationException"))
		{
			return make_exception(documents::compilation::IndexCompilationException(errorMessage), inner);
		}
		////
		if (type_contains("CounterOverflowException"))
		{
			return make_exception(documents::counters::CounterOverflowException(errorMessage), inner);
		}
		////
		if (type_contains("IndexAlreadyExistException"))
		{
			return make_exception(documents::indexes::IndexAlreadyExistException(errorMessage), inner);
		}
		if (type_contains("IndexCreationException"))
		{
			return make_exception(documents::indexes::IndexCreationException(errorMessage), inner);
		}
		if (type_contains("IndexDeletionException"))
		{
			return make_exception(documents::indexes::IndexDeletionException(errorMessage), inner);
		}
		if (type_contains("IndexDoesNotExistException"))
		{
			return make_exception(documents::indexes::IndexDoesNotExistException(errorMessage), inner);
		}
		if (type_contains("IndexInvalidException"))
		{
			return make_exception(documents::indexes::IndexInvalidException(errorMessage), inner);
		}
		////
		if (type_contains("JavaScriptException"))
		{
			return make_exception(documents::patching::JavaScriptException(errorMessage), inner);
		}
		if (type_contains("JavaScriptParseException"))
		{
			return make_exception(documents::patching::JavaScriptParseException(errorMessage), inner);
		}
		////
		if (type_contains("RevisionsDisabledException"))
		{
			return make_exception(documents::revisions::RevisionsDisabledException(errorMessage), inner);
		}
		////
		if (type_contains("NonUniqueObjectException"))
		{
			return make_exception(documents::session::NonUniqueObjectException(errorMessage), inner);
		}
		////
		if (type_contains("SubscriberErrorException"))
		{
			return make_exception(documents::subscriptions::SubscriberErrorException(errorMessage), inner);
		}
		if (type_contains("SubscriptionChangeVectorUpdateConcurrencyException"))
		{
			return make_exception(documents::subscriptions::SubscriptionChangeVectorUpdateConcurrencyException(errorMessage), inner);
		}
		if (type_contains("SubscriptionClosedException"))
		{
			return make_exception(documents::subscriptions::SubscriptionClosedException(errorMessage), inner);
		}
		if (type_contains("SubscriptionDoesNotBelongToNodeException"))
		{
			return make_exception(documents::subscriptions::SubscriptionDoesNotBelongToNodeException(errorMessage), inner);
		}
		if (type_contains("SubscriptionDoesNotExistException"))
		{
			return make_exception(documents::subscriptions::SubscriptionDoesNotExistException(errorMessage), inner);
		}
		if (type_contains("SubscriptionInUseException"))
		{
			return make_exception(documents::subscriptions::SubscriptionInUseException(errorMessage), inner);
		}
		if (type_contains("SubscriptionInvalidStateException"))
		{
			return make_exception(documents::subscriptions::SubscriptionInvalidStateException(errorMessage), inner);
		}
		////
		if (type_contains("RouteNotFoundException"))
		{
			return make_exception(routing::RouteNotFoundException(errorMessage), inner);
		}
		////
		if (type_contains("AuthenticationException"))
		{
			return make_exception(security::AuthenticationException(errorMessage), inner);
		}
		if (type_contains("AuthorizationException"))
		{
			return make_exception(security::AuthorizationException(errorMessage), inner);
		}
		if (type_contains("CertificateNameMismatchException"))
		{
			return make_exception(security::CertificateNameMismatchException(errorMessage), inner);
		}
		if (type_contains("SecurityException"))
		{
			return make_exception(security::SecurityException(errorMessage), inner);
		}
		////
		if (type_contains("ServerLoadFailureException"))
		{
			return make_exception(server::ServerLoadFailureException(errorMessage), inner);
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
			const auto& json = response.output.str();
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
