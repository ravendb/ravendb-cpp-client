#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "GeekPerson.h"
#include "AdvancedSessionOperations.h"
#include "EntityIdHelperUtil.h"

namespace ravendb::client::tests::client
{
	class LoadTests : public driver::RavenTestDriver
	{
	protected:
		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}
	};

	TEST_F(LoadTests, LoadCanUseCache)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			ASSERT_TRUE(user);
			ASSERT_EQ(std::string("RavenDB"), user->name);

			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			auto user2 = session.load<infrastructure::entities::User>("users/1");

			ASSERT_TRUE(user2);
			ASSERT_EQ(std::string("RavenDB"), user2->name);

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(LoadTests, LoadDocumentById)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			ASSERT_TRUE(user);
			ASSERT_EQ(std::string("RavenDB"), user->name);
		}
	}

	TEST_F(LoadTests, LoadDocumentsByIds)
	{
		auto store = get_document_store(TEST_NAME);
		std::vector<std::string> ids = { "users/1", "users/2" };
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1, ids[0]);
			session.store(user2, ids[1]);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto users = session.load<infrastructure::entities::User>(ids);
			ASSERT_EQ(2, users.size());
		}
	}

	TEST_F(LoadTests, LoadNullShouldReturnNull)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1);
			session.store(user2);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user1 = session.load<infrastructure::entities::User>(std::string());
			ASSERT_FALSE(user1);
		}
	}

	TEST_F(LoadTests, LoadMultiIdsWithNullShouldReturnDictionaryWithoutNulls)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1, "users/1");
			session.store(user2, "users/2");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::vector<std::string> ordered_array_of_ids_with_empty = { "users/1", {}, "users/2", {} };
			auto users1 = session.load<infrastructure::entities::User>(ordered_array_of_ids_with_empty.begin(), ordered_array_of_ids_with_empty.end());

			auto user1 = users1.at("users/1");
			auto user2 = users1.at("users/2");

			ASSERT_TRUE(user1);
			ASSERT_TRUE(user2);

			auto unordered_set_of_ids_with_empty = std::unordered_set<std::string>(
				ordered_array_of_ids_with_empty.cbegin(), ordered_array_of_ids_with_empty.cend());

			auto users2 = session.load<infrastructure::entities::User>(unordered_set_of_ids_with_empty.begin(), unordered_set_of_ids_with_empty.end());
			ASSERT_EQ(2, users2.size());
		}
	}

	TEST_F(LoadTests, LoadDocumentWithIntsArrayAndLongsArray)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto geek1 = std::make_shared<infrastructure::entities::GeekPerson>();
			geek1->name = "Moshe";
			geek1->favorite_primes = { 13, 43, 443, 997 };
			geek1->favorite_very_large_primes = { 5000000029L, 5000000039L };
			session.store(geek1, "geeks/1");

			auto geek2 = std::make_shared<infrastructure::entities::GeekPerson>();
			geek2->name = "Itzik";
			geek2->favorite_primes = { 2, 3, 5, 7 };
			geek2->favorite_very_large_primes = { 999999999989L };
			session.store(geek2, "geeks/2");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto geek1 = session.load<infrastructure::entities::GeekPerson>("geeks/1");
			auto geek2 = session.load<infrastructure::entities::GeekPerson>("geeks/2");

			ASSERT_EQ(43, geek1->favorite_primes[1]);
			ASSERT_EQ(5000000039L, geek1->favorite_very_large_primes[1]);

			ASSERT_EQ(7, geek2->favorite_primes[3]);
			ASSERT_EQ(999999999989L, geek2->favorite_very_large_primes[0]);
		}
	}

	TEST_F(LoadTests, ShouldLoadManyIdsAsPostRequest)
	{
		auto store = get_document_store(TEST_NAME);

		std::vector<std::string> ids{};
		{
			auto session = store->open_session();
			// Length of all the ids together should be larger than 1024 for POST request
			for (int i = 0; i < 200; ++i)
			{
				auto id = "users/" + std::to_string(i);
				ids.emplace_back(id);

				auto user = std::make_shared<infrastructure::entities::User>();
				user->name = "Person " + std::to_string(i);
				session.store(user, id);
			}
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto users = session.load<infrastructure::entities::User>(ids.begin(), ids.end());
			auto user77 = users.at(std::string("users/77"));

			ASSERT_TRUE(user77);

			ASSERT_EQ("users/77", user77->id);
		}
	}

	//TODO implement
	TEST_F(LoadTests, LoadStartsWith)
	{
		SUCCEED();
	}
}
