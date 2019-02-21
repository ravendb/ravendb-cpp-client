#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "SessionOptions.h"
#include "User.h"

namespace ravendb::client::tests
{
	class BasicDocumentsSessionTests : public infrastructure::TestSuiteBase
	{
	protected:
		static const infrastructure::entities::User example_user;

		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}

		static void TearDownTestCase()
		{
			test_suite_store.reset();
		}
	};

	const infrastructure::entities::User BasicDocumentsSessionTests::example_user{ "users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };


	TEST_F(BasicDocumentsSessionTests, SessionCRUDTest)
	{
		std::string doc_id = "users/1";

		const auto session_options = documents::session::SessionOptions{ test_suite_store->get().get_database(),
			false, true, {},
			documents::session::TransactionMode::SINGLE_NODE };
		{
			auto session = test_suite_store->get().open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_FALSE(user);
		}
		{
			auto session = test_suite_store->get().open_session(session_options);
			auto user = std::make_shared<infrastructure::entities::User>(example_user);
			session.store(user, user->id);
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_TRUE(user);
			ASSERT_EQ(user->name, example_user.name);
			ASSERT_EQ(user->last_name, example_user.last_name);
		}
		{
			auto session = test_suite_store->get().open_session(session_options);
			session.delete_document(example_user.id);
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_FALSE(user);
		}
	}

	
}
