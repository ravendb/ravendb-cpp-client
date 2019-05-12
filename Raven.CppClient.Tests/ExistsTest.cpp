#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::tests::client
{
	class ExistsTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(ExistsTest, CheckIfDocumentExists)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1, "users/1");

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Sasha";
			session.store(user2, "users/2");

			session.save_changes();
		}

		auto session = store->open_session();

		ASSERT_TRUE(session.advanced().exists("users/1"));

		ASSERT_FALSE(session.advanced().exists("users/12"));

		auto user = session.load<infrastructure::entities::User>("users/2");

		ASSERT_TRUE(session.advanced().exists("users/2"));
	}
}
