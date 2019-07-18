#pragma once
#include <shared_mutex>
#include <map>
#include "DocumentStoreBase.h"
#include "CompareStringsLessThanIgnoreCase.h"
#include "TasksScheduler.h"

namespace ravendb::client::documents::identity
{
	class MultiDatabaseHiLoIdGenerator;
}

namespace ravendb::client::documents
{
	//Manages access to RavenDB and open sessions to work with RavenDB.
	class DocumentStore : public DocumentStoreBase
	{
	private:
		struct Deleter
		{
			void operator()(DocumentStore* ptr) const;
		};

	private:
		std::shared_ptr<primitives::IExecutorService> _executor_service{};
		std::shared_ptr<impl::TasksScheduler> _scheduler{};

		mutable std::map<std::string, std::shared_ptr<http::RequestExecutor>, impl::utils::CompareStringsLessThanIgnoreCase>
			_request_executors{};
		mutable std::shared_mutex _request_executors_mutex{};

		std::unique_ptr<identity::MultiDatabaseHiLoIdGenerator> _multi_db_hilo;

		std::shared_ptr<operations::MaintenanceOperationExecutor> _maintenance_operation_executor{};	
		std::shared_ptr<operations::OperationExecutor> _operation_executor{};			

		std::string _identifier{};

		std::vector<primitives::EventHandler> after_close{};
		std::vector<primitives::EventHandler> before_close{};

	private:
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

		std::shared_ptr<primitives::IExecutorService> get_executor_service() const;

		void set_executor_service(std::shared_ptr<primitives::IExecutorService> executor_service);

		void set_default_executor_service(uint32_t num_of_threads = 1);

		std::shared_ptr<impl::TasksScheduler> get_scheduler() const;

		std::string get_identifier() const override;

		void set_identifier(std::string identifier) override;

		void close() override;

		session::DocumentSession open_session() override;
		session::DocumentSession open_session(const std::string& database) override;
		session::DocumentSession open_session(const session::SessionOptions& options) override;

		std::shared_ptr<http::RequestExecutor> get_request_executor(const std::string& database = {}) const override;

		//Initializes this instance.
		std::shared_ptr<IDocumentStore> initialize() override;

		void add_before_close_listener(primitives::EventHandler handler) override;

		void remove_before_close_listener(const primitives::EventHandler& handler) override;

		void add_after_close_listener(primitives::EventHandler handler) override;

		void remove_after_close_listener(const primitives::EventHandler& handler) override;

		std::shared_ptr<operations::MaintenanceOperationExecutor> maintenance() override;

		std::shared_ptr<operations::OperationExecutor> operations() override;
	};
}
