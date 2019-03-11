#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "User.h"

namespace ravendb::client::tests::client
{
	class TrackEntityTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(TrackEntityTest, DeletingEntityThatIsNotTrackedShouldThrow)
	{
		{
			auto session = test_suite_store->get()->open_session();
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
		{
			auto session = test_suite_store->get()->open_session();

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
			auto session = test_suite_store->get()->open_session();
			session.delete_document("users/1");
			session.delete_document("users/2");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();

			ASSERT_FALSE(session.load<infrastructure::entities::User>("users/1"));
			ASSERT_FALSE(session.load<infrastructure::entities::User>("users/2"));
		}
	}

	TEST_F(TrackEntityTest, StoringDocumentWithTheSameIdInTheSameSessionShouldThrow)
	{
		auto session = test_suite_store->get()->open_session();

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
		catch (std::runtime_error& e)
		{
			std::string_view expected_msg_start = "Attempted to associate a different object with id 'users/1'";
			std::string_view msg = e.what();
			ASSERT_EQ(0, msg.substr(0, expected_msg_start.length()).compare(expected_msg_start));
		}
	}

}