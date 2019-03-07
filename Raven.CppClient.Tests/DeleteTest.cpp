#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "User.h"


namespace ravendb::client::tests::client
{
	class DeleteTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(DeleteTest, DeleteDocumentByEntity)
	{
		auto session = test_suite_store->get()->open_session();

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
		auto session = test_suite_store->get()->open_session();

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
