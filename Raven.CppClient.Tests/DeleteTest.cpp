#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"


namespace ravendb::client::tests::client
{
	class DeleteTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(DeleteTest, DeleteDocumentByEntity)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "RavenDB";
		session.store(user, "users/1");
		session.save_changes();

		user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_TRUE(user);

		session.delete_document(user);
		session.save_changes();

		auto null_user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_FALSE(null_user);
	}

	TEST_F(DeleteTest, DeleteDocumentById)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "RavenDB";
		session.store(user, "users/1");
		session.save_changes();

		user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_TRUE(user);

		session.delete_document("users/1");
		session.save_changes();

		auto null_user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_FALSE(null_user);
	}
}
