#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "MaintenanceOperationExecutor.h"
#include "CreateDatabaseOperation.h"
#include "ToggleDatabasesStateOperation.h"
#include "GetDatabaseRecordOperation.h"
#include "DeleteDatabasesOperation.h"
#include "AddDatabaseNodeOperation.h"

namespace ravendb::client::tests::server
{
	class DatabasesTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(DatabasesTest, CanDisableAndEnableDatabase)
	{
		auto store = get_document_store(TEST_NAME);

		auto db_record = serverwide::DatabaseRecord("enableDisable");
		auto database_operation = serverwide::operations::CreateDatabaseOperation(db_record);
		store->maintenance()->server()->send(database_operation);

		auto toggle_result = store->maintenance()->server()->send(
			documents::operations::ToggleDatabasesStateOperation("enableDisable", true));

		ASSERT_TRUE(toggle_result);
		ASSERT_FALSE(toggle_result->name.empty());

		auto disabled_database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(
			"enableDisable"));

		ASSERT_TRUE(disabled_database_record->disabled);

		// now enable database

		toggle_result = store->maintenance()->server()->send(documents::operations::ToggleDatabasesStateOperation(
			"enableDisable", false));

		ASSERT_TRUE(toggle_result);
		ASSERT_FALSE(toggle_result->name.empty());

		auto enabled_database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(
			"enableDisable"));
		ASSERT_FALSE(enabled_database_record->disabled);

		store->maintenance()->server()->send(serverwide::operations::DeleteDatabasesOperation("enableDisable", true));
	}

	TEST_F(DatabasesTest, CanAddNode)
	{
		auto store = get_document_store(TEST_NAME);

		auto result = store->maintenance()->server()->send(serverwide::operations::AddDatabaseNodeOperation(
			store->get_database()));

		ASSERT_GT(result->raft_command_index, 0);
		ASSERT_EQ(2, result->topology.replication_factor);
	}
}
