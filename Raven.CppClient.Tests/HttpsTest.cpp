#include "pch.h"
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "User.h"


namespace ravendb::client::tests::client
{
	class HttpsTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_SECURED_DOCUMENT_STORE();
		}
	};

	TEST_F(HttpsTest, CanConnectWithCertificate)
	{
		{
			auto session = test_suite_store->get()->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			ASSERT_TRUE(user);
		}

	}
}