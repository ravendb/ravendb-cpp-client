#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"

namespace ravendb::client::tests::client
{
	class HttpsTest : public driver::RavenTestDriver
	{
		protected:
		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}
	};

	TEST_F(HttpsTest, CanConnectWithCertificate)
	{
		auto store = get_secured_document_store(TEST_NAME);

		std::string user_id{};
		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user);
			user_id = user->id;
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>(user_id);
			ASSERT_TRUE(user);
		}
	}
}
