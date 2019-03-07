#pragma once

namespace ravendb::client
{
	namespace http
	{
		class RequestExecutor;
	}

	namespace documents
	{
		namespace session
		{
			struct SessionOptions;
			class DocumentSession;
		}

		namespace operations
		{
			class OperationExecutor;
			class MaintenanceOperationExecutor;
		}

		namespace conventions
		{
			class DocumentConventions;
		}
	}
}

namespace  ravendb::client::documents
{
	//TODO complete!
	struct IDocumentStore
	{
		virtual ~IDocumentStore() = 0;

		virtual std::string get_identifier() const = 0;

		virtual void set_identifier(std::string identifier) = 0;

		virtual std::shared_ptr<IDocumentStore> initialize() = 0;

		virtual session::DocumentSession open_session() = 0;
		virtual session::DocumentSession open_session(const std::string& database) = 0;
		virtual session::DocumentSession open_session(const session::SessionOptions& options) = 0;

		virtual std::shared_ptr<conventions::DocumentConventions> get_conventions() const = 0;

		virtual const std::vector<std::string>& get_urls() const = 0;

		virtual const std::string& get_database() const = 0;

		virtual std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const = 0;

		virtual std::shared_ptr<operations::MaintenanceOperationExecutor> get_maintenance() const = 0;

		virtual std::shared_ptr<operations::OperationExecutor> get_operations() const = 0;			
	};

	inline IDocumentStore::~IDocumentStore() = default;
}
