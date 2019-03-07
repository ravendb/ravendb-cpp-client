#pragma once
#include <shared_mutex>
#include "DocumentStoreBase.h"
#include "CompareStringsIgnoreCase.h"

namespace ravendb::client::documents
{
	class DocumentStore : public DocumentStoreBase
	{
	private:
		mutable std::map<std::string, std::shared_ptr<http::RequestExecutor>, impl::utils::CompareStringsIgnoreCase> _request_executors{};
		mutable std::shared_mutex _request_executors_mutex{};

		std::shared_ptr<operations::MaintenanceOperationExecutor> _maintenance_operation_executor{};	
		std::shared_ptr<operations::OperationExecutor> _operation_executor{};			

		std::string _identifier{};

		std::weak_ptr<DocumentStore> _weak_this;

		void initiate_operation_executors();

		DocumentStore();

		DocumentStore(std::string url, std::string database);

		DocumentStore(std::vector<std::string> urls, std::string database);

	protected:
		void assert_valid_configuration() const;

	public:
		~DocumentStore() override;

		static std::shared_ptr<DocumentStore> create();

		static std::shared_ptr<DocumentStore> create(std::string url, std::string database);

		static std::shared_ptr<DocumentStore> create(std::vector<std::string> urls, std::string database);

		std::string get_identifier() const override;

		void set_identifier(std::string identifier) override;

		session::DocumentSession open_session() override;
		session::DocumentSession open_session(const std::string& database) override;
		session::DocumentSession open_session(const session::SessionOptions& options) override;

		//can be called with empty database param
		std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const override;

		std::shared_ptr<IDocumentStore> initialize() override;

		std::shared_ptr<operations::MaintenanceOperationExecutor> get_maintenance() const override;

		std::shared_ptr<operations::OperationExecutor> get_operations() const override;
	};
}
