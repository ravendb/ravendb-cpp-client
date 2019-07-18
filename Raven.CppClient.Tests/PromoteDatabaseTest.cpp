#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "MaintenanceOperationExecutor.h"
#include "AddDatabaseNodeOperation.h"
#include "PromoteDatabaseNodeOperation.h"
#include "GetDatabaseRecordOperation.h"

namespace ravendb::client::tests::server
{
	class PromoteDatabaseTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(PromoteDatabaseTest, CanSendPromoteDatabaseCommand)
	{
		auto store = get_document_store(TEST_NAME);

		auto result = store->maintenance()->server()->send(serverwide::operations::AddDatabaseNodeOperation(
			store->get_database()));

		auto added_node = result->topology.promotables.at(0);

		store->maintenance()->server()->send(serverwide::operations::PromoteDatabaseNodeOperation(
			store->get_database(), added_node));

		auto promotion_result = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(
			store->get_database()));
		const auto& new_members = promotion_result->topology.members;

		ASSERT_TRUE(std::find(new_members.cbegin(), new_members.cend(), added_node) !=
			new_members.cend());
	}
}
