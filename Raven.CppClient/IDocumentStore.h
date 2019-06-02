#pragma once
#include <optional>
#include "IDisposalNotification.h"

namespace ravendb::client
{
	namespace impl
	{
		struct CertificateDetails;
	}
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
		namespace indexes
		{
			class AbstractIndexCreationTaskBase;
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
	//Interface for managing access to RavenDB and open sessions.
	class IDocumentStore : public impl::IDisposalNotification
	{
	public:
		virtual ~IDocumentStore() = 0;

		virtual const std::optional<impl::CertificateDetails>& get_certificate_details() const = 0;

		virtual void add_before_store_listener(primitives::EventHandler handler) = 0;
		virtual void remove_before_store_listener(const primitives::EventHandler& handler) = 0;

		virtual void add_after_save_changes_listener(primitives::EventHandler handler) = 0;
		virtual void remove_after_save_changes_listener(const primitives::EventHandler& handler) = 0;

		virtual void add_before_delete_listener(primitives::EventHandler handler) = 0;
		virtual void remove_before_delete_listener(const primitives::EventHandler& handler) = 0;

		virtual void add_before_query_listener(primitives::EventHandler handler) = 0;
		virtual void remove_before_query_listener(const primitives::EventHandler& handler) = 0;


		virtual std::string get_identifier() const = 0;

		virtual void set_identifier(std::string identifier) = 0;

		virtual std::shared_ptr<IDocumentStore> initialize() = 0;

		virtual session::DocumentSession open_session() = 0;
		virtual session::DocumentSession open_session(const std::string& database) = 0;
		virtual session::DocumentSession open_session(const session::SessionOptions& options) = 0;

		virtual void execute_index(std::shared_ptr<indexes::AbstractIndexCreationTaskBase> task,
			std::optional<std::string> database = {}) = 0;
		virtual void execute_indexes(std::vector<std::shared_ptr<indexes::AbstractIndexCreationTaskBase>>& tasks,
			std::optional<std::string> database = {}) = 0;

		virtual std::shared_ptr<conventions::DocumentConventions> get_conventions() const = 0;

		virtual const std::vector<std::string>& get_urls() const = 0;

		virtual const std::string& get_database() const = 0;

		virtual std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const = 0;

		virtual std::shared_ptr<operations::MaintenanceOperationExecutor> maintenance() = 0;

		virtual std::shared_ptr<operations::OperationExecutor> operations() = 0;			
	};

	inline IDocumentStore::~IDocumentStore() = default;
}
