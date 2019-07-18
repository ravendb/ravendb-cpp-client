#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "User.h"
#include "MaintenanceOperationExecutor.h"
#include "CompactDatabaseOperation.h"

namespace ravendb::client::tests::client
{
	class CompactTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::User, id);
		}
	};

	TEST_F(CompactTest, CanCompactDatabase)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->lastName = "user1";
			session.store(user, "users/1");
			session.save_changes();
		}

		auto compact_settings = serverwide::CompactSettings();
		compact_settings.database_name = store->get_database();
		compact_settings.documents = true;
		compact_settings.node = store->get_request_executor()->get_topology()->nodes->at(0);		
		
		auto operation = store->maintenance()->server()->send_async(documents::operations::CompactDatabaseOperation(compact_settings));

		operation->wait_for_completion();
	}
}
