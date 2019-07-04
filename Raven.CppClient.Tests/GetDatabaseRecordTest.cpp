#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "MaintenanceOperationExecutor.h"
#include "GetDatabaseRecordOperation.h"

namespace ravendb::client::tests::serverwide_commands
{
	class GetDatabaseRecordTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(GetDatabaseRecordTest, CanGetDatabaseRecord)
	{
		auto store = get_document_store(TEST_NAME);

		auto database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(
			store->get_database()));

		ASSERT_TRUE(database_record);
		ASSERT_EQ(store->get_database(), database_record->database_name);
	}
}
