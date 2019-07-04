#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "NonUniqueObjectException.h"

namespace ravendb::client::tests::client
{
	class TrackEntityTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}
	};

	TEST_F(TrackEntityTest, DeletingEntityThatIsNotTrackedShouldThrow)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			try
			{
				session.delete_document(std::make_shared<infrastructure::entities::User>());
			}
			catch (std::runtime_error& e)
			{
				std::string_view expected_msg_end = "is not associated with the session, cannot delete unknown entity instance";
				std::string_view msg = e.what();
				ASSERT_EQ(0, msg.substr(msg.length() - expected_msg_end.length()).compare(expected_msg_end));
			}
		}
	}

	TEST_F(TrackEntityTest, LoadingDeletedDocumentShouldReturnNull)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Svyatoslav";
			user1->id = "users/1";

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Izyaslav";
			user2->id = "users/2";

			session.store(user1);
			session.store(user2);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.delete_document("users/1");
			session.delete_document("users/2");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			ASSERT_FALSE(session.load<infrastructure::entities::User>("users/1"));
			ASSERT_FALSE(session.load<infrastructure::entities::User>("users/2"));
		}
	}

	TEST_F(TrackEntityTest, StoringDocumentWithTheSameIdInTheSameSessionShouldThrow)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->id = "users/1";
		user->name = "User1";

		session.store(user);
		session.save_changes();

		auto new_user = std::make_shared<infrastructure::entities::User>();
		new_user->id = "users/1";
		new_user->name = "User2";

		try
		{
			session.store(new_user);
		}
		catch (exceptions::documents::session::NonUniqueObjectException& e)
		{
			std::string_view expected_msg_start = "Attempted to associate a different object with id 'users/1'";
			std::string_view msg = e.what();
			ASSERT_EQ(0, msg.substr(0, expected_msg_start.length()).compare(expected_msg_start));
			return;
		}
		FAIL();
	}

}
