#include "pch.h"
#include "definitions.h"
#include "User.h"
#include "DocumentSession.h"

namespace ravendb::client::tests
{
	class BasicDocumentsSessionTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<RequestExecutorScope> test_suite_executor{};

		static const User example_user;

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
		}
	};

	const User BasicDocumentsSessionTests::example_user{ "users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };


	TEST_F(BasicDocumentsSessionTests, SessionCRUDTest)
	{
		std::string doc_id = "users/1";

		const auto session_options = documents::session::SessionOptions{ test_suite_executor->get_db_name(),
			false, true, test_suite_executor->get_shared(), 
			documents::session::TransactionMode::SINGLE_NODE };
		{
			documents::session::DocumentSession session(session_options);
			auto user = session.load<User>(example_user.id);
			ASSERT_FALSE(user);
		}
		{
			documents::session::DocumentSession session(session_options);
			auto user = std::make_shared<User>(example_user);
			session.store(user, user->id);
			session.save_changes();
		}
		{
			documents::session::DocumentSession session(session_options);
			auto user = session.load<User>(example_user.id);
			ASSERT_TRUE(user);
			ASSERT_EQ(user->first_name, example_user.first_name);
			ASSERT_EQ(user->last_name, example_user.last_name);
		}
		{
			documents::session::DocumentSession session(session_options);
			session.delete_document(example_user.id);
			session.save_changes();
		}
		{
			documents::session::DocumentSession session(session_options);
			auto user = session.load<User>(example_user.id);
			ASSERT_FALSE(user);
		}
	}

	
}
