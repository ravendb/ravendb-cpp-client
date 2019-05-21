#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "RequestExecutor.h"
#include "GetNextOperationIdCommand.h"
#include "GetDatabaseNamesOperation.h"
#include "AllTopologyNodesDownException.h"

namespace ravendb::client::tests::client
{
	class RequestExecutorTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RequestExecutorTest, FailuresDoesNotBlockConnectionPool)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create(store->get_urls(), "no_such_db", {},
			store->get_scheduler(),	conventions, store->get_before_perform(), store->get_after_perform());

		int32_t errors_count = 0;

		for(auto i = 0; i < 40; ++i)
		{
			try
			{
				auto command = documents::commands::GetNextOperationIdCommand();
				executor->execute(command);
			}
			catch (...)
			{
				++errors_count;
			}
		}

		ASSERT_EQ(40, errors_count);

		ASSERT_THROW(
		{
			auto database_names_operation = serverwide::operations::GetDatabaseNamesOperation(0, 20);
			auto command = database_names_operation.get_command(conventions);
			executor->execute(*command);
		}, ravendb::client::exceptions::database::DatabaseDoesNotExistException);
		
	}

	TEST_F(RequestExecutorTest, CanIssueManyRequests)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create(store->get_urls(), store->get_database(), {},
			store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

		for (auto i = 0; i < 50; ++i)
		{
			auto operation = serverwide::operations::GetDatabaseNamesOperation(0, 20);
			auto command = operation.get_command(conventions);
			executor->execute(*command);
		}
	}

	TEST_F(RequestExecutorTest, canFetchDatabasesNames)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create(store->get_urls(), store->get_database(), {},
			store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

		auto operation = serverwide::operations::GetDatabaseNamesOperation(0, 20);
		auto command = operation.get_command(conventions);
		executor->execute(*command);

		auto result = command->get_result();
		ASSERT_TRUE(std::find(result->begin(), result->end(), store->get_database()) != result->end());
	}

	TEST_F(RequestExecutorTest, ThrowsWhenUpdatingTopologyOfNotExistingDb)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create(store->get_urls(), "no_such_db", {},
			store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

		auto server_node = std::make_shared<http::ServerNode>();
		server_node->url = store->get_urls()[0];
		server_node->database = "no_such";

		ASSERT_THROW(executor->update_topology_async(server_node, 5000).get(),
			exceptions::database::DatabaseDoesNotExistException);
	}

	TEST_F(RequestExecutorTest, ThrowsWhenDatabaseDoesNotExist)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create(store->get_urls(), "no_such_db", {},
			store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

		auto command = documents::commands::GetNextOperationIdCommand();

		ASSERT_THROW(executor->execute(command),
			ravendb::client::exceptions::database::DatabaseDoesNotExistException);
	}

	TEST_F(RequestExecutorTest, CanCreateSingleNodeRequestExecutor)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		auto executor = http::RequestExecutor::create_for_single_node_without_configuration_updates(store->get_urls()[0],
			store->get_database(), {},
			store->get_scheduler(), conventions,
			store->get_before_perform(), store->get_after_perform());

		{
			auto nodes = executor->get_topology_nodes();
			ASSERT_EQ(1, nodes->size());

			auto server_node = nodes->at(0);
			ASSERT_EQ(store->get_urls()[0], server_node->url);
			ASSERT_EQ(store->get_database(), server_node->database);
		}
		auto command = documents::commands::GetNextOperationIdCommand();
		executor->execute(command);
		ASSERT_TRUE(command.get_result());

		{
			auto nodes = executor->get_topology_nodes();
			ASSERT_EQ(1, nodes->size());

			auto server_node = nodes->at(0);
			ASSERT_EQ(store->get_database(), server_node->database);
		}
	}

	TEST_F(RequestExecutorTest, CanChooseOnlineNode)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);
		const auto& url = store->get_urls()[0];
		const auto& db_name = store->get_database();

		auto executor = http::RequestExecutor::create({ "http://no_such_host:8080", "http://another_offlilne:8080", url },
			db_name, {},
			store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

		auto command = documents::commands::GetNextOperationIdCommand();
		executor->execute(command);

		ASSERT_TRUE(command.get_result());

		auto topology_nodes = executor->get_topology_nodes();

		ASSERT_EQ(1, topology_nodes->size());
		ASSERT_EQ(db_name, topology_nodes->at(0)->database);
	}

	TEST_F(RequestExecutorTest, FailsWhenServerIsOffline)
	{
		auto conventions = documents::conventions::DocumentConventions::default_conventions();

		auto store = get_document_store(TEST_NAME);

		ASSERT_THROW(
		{
			auto executor = http::RequestExecutor::create({ "http://no_such_host:8080" }, "db1", {},
				store->get_scheduler(), conventions, store->get_before_perform(), store->get_after_perform());

			auto command = documents::commands::GetNextOperationIdCommand();

			executor->execute(command);
		}, ravendb::client::exceptions::AllTopologyNodesDownException);
	}
}
