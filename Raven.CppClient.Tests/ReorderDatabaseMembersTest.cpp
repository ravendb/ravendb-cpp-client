#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "MaintenanceOperationExecutor.h"
#include "ReorderDatabaseMembersOperation.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::tests::client::documents::commands
{
	class ReorderDatabaseMembersTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(ReorderDatabaseMembersTest, CanSendReorderCommand)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		store->maintenance()->server()->send(serverwide::operations::configuration::ReorderDatabaseMembersOperation(
			store->get_database(), { session.advanced().get_current_session_node()->cluster_tag }));
	}
}
