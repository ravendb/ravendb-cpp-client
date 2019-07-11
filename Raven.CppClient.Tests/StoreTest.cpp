#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"

namespace ravendb::client::tests::client
{
	class StoreTest : public driver::RavenTestDriver
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

	TEST_F(StoreTest, DISABLED_RefreshTest)
	{
		//TODO wait for session.advanced().refresh(user) implementation
	}

	TEST_F(StoreTest, StoreDocument)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "RavenDB";
		session.store(user, "users/1");

		user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_TRUE(user);
		ASSERT_EQ("RavenDB", user->name);
	}

	TEST_F(StoreTest, StoreDocuments)
	{
		auto store = get_document_store(TEST_NAME);

		constexpr size_t NUM_OF_USERS = 50;
		std::vector<std::string> users_ids{};
		users_ids.reserve(NUM_OF_USERS);

		{
			auto session = store->open_session();
			for (size_t i = 1; i <= NUM_OF_USERS; ++i)
			{
				auto user = std::make_shared<infrastructure::entities::User>();
				user->name = "RavenDB" + std::to_string(i);
				users_ids.push_back(user->id = "users/" + std::to_string(i));
				session.store(user);
			}
			session.save_changes();

			auto loaded_users = session.load<infrastructure::entities::User>(users_ids);
			ASSERT_EQ(NUM_OF_USERS, loaded_users.size());
			for (const auto& user : loaded_users)
			{
				ASSERT_TRUE(user.second);
			}
		}
		{
			auto session = store->open_session();
			auto loaded_users = session.load<infrastructure::entities::User>(users_ids);
			ASSERT_EQ(NUM_OF_USERS, loaded_users.size());
			for (const auto& user : loaded_users)
			{
				ASSERT_TRUE(user.second);
			}
		}
	}

	TEST_F(StoreTest, DISABLED_NotifyAfterStore)
	{
		//TODO wait for implementation of AfterSaveChangesEventArgs
	}
}