#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "SessionOptions.h"
#include "User.h"

namespace ravendb::client::tests::old_tests
{
	class BasicDocumentsSessionTests : public driver::RavenTestDriver
	{
	protected:
		static const infrastructure::entities::User example_user;

		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	const infrastructure::entities::User BasicDocumentsSessionTests::example_user{ "users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };


	TEST_F(BasicDocumentsSessionTests, SessionCRUDTest)
	{
		auto store = get_document_store(TEST_NAME);

		std::string doc_id = "users/1";

		const auto session_options = documents::session::SessionOptions{ store->get_database(),
			false, true, {},
			documents::session::TransactionMode::SINGLE_NODE };
		{
			auto session = store->open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_FALSE(user);
		}
		{
			auto session = store->open_session(session_options);
			auto user = std::make_shared<infrastructure::entities::User>(example_user);
			session.store(user, user->id);
			session.save_changes();
		}
		{
			auto session = store->open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_TRUE(user);
			ASSERT_EQ(user->name, example_user.name);
			ASSERT_EQ(user->last_name, example_user.last_name);
		}
		{
			auto session = store->open_session(session_options);
			session.delete_document(example_user.id);
			session.save_changes();
		}
		{
			auto session = store->open_session(session_options);
			auto user = session.load<infrastructure::entities::User>(example_user.id);
			ASSERT_FALSE(user);
		}
	}

	
}
