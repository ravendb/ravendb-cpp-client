#pragma once
#include <shared_mutex>
#include "DocumentStoreBase.h"
#include "utils.h"

namespace ravendb::client::documents
{
	class DocumentStore : public DocumentStoreBase
	{
	private:
		mutable std::map<std::string, std::shared_ptr<http::RequestExecutor>, impl::utils::CompareStringsIgnoreCase> _request_executors{};
		mutable std::shared_mutex _request_executors_mutex{};

		std::shared_ptr<operations::MaintenanceOperationExecutor> _maintenance_operation_executor = 
			std::make_shared<operations::MaintenanceOperationExecutor>(*this);
		std::shared_ptr<operations::OperationExecutor> _operation_executor =
			std::make_shared<operations::OperationExecutor>(*this);

		std::string _identifier{};

	protected:
		void assert_valid_configuration() const;

	public:
		~DocumentStore() override;

		DocumentStore();

		DocumentStore(std::string url, std::string database);

		DocumentStore(std::vector<std::string> urls, std::string database);

		std::string get_identifier() const override;

		void set_identifier(std::string identifier) override;

		session::DocumentSession open_session() override;
		session::DocumentSession open_session(const std::string& database) override;
		session::DocumentSession open_session(const session::SessionOptions& options) override;

		//can be called with empty database param
		std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const override;

		std::reference_wrapper<IDocumentStore> initialize() override;

		std::shared_ptr<operations::MaintenanceOperationExecutor> get_maintenance() const override;

		std::shared_ptr<operations::OperationExecutor> get_operations() const override;
	};
}
