#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "User.h"
#include "GeekPerson.h"

namespace ravendb::client::tests::client
{
	class LoadTests : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	//TODO implement
	TEST_F(LoadTests, LoadCanUseCache)
	{
		SUCCEED();
	}

	TEST_F(LoadTests, LoadDocumentById)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			ASSERT_TRUE(user);
			ASSERT_EQ(std::string("RavenDB"), user->name);
		}
	}

	TEST_F(LoadTests, LoadDocumentsByIds)
	{
		std::vector<std::string> ids = { "users/1", "users/2" };
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1, ids[0]);
			session.store(user2, ids[1]);
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto users = session.load<infrastructure::entities::User>(ids);
			ASSERT_EQ(2, users.size());
		}
	}

	//rewrite using id generator
	TEST_F(LoadTests, LoadNullShouldReturnNull)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1, "users/1");
			session.store(user2, "users/2");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = session.load<infrastructure::entities::User>(std::string());
			ASSERT_FALSE(user1);
		}
	}

	TEST_F(LoadTests, LoadMultiIdsWithNullShouldReturnDictionaryWithoutNulls)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "RavenDB";
			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Hibernating Rhinos";

			session.store(user1, "users/1");
			session.store(user2, "users/2");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

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
		{
			auto session = test_suite_store->get().open_session();

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
			auto session = test_suite_store->get().open_session();

			auto geek1 = session.load<infrastructure::entities::GeekPerson>("geeks/1");
			auto geek2 = session.load<infrastructure::entities::GeekPerson>("geeks/2");

			ASSERT_EQ(43, geek1->favorite_primes[1]);
			ASSERT_EQ(5000000039L, geek1->favorite_very_large_primes[1]);

			ASSERT_EQ(7, geek2->favorite_primes[3]);
			ASSERT_EQ(999999999989L, geek2->favorite_very_large_primes[0]);
		}
	}

	//TODO originally id is checked
	TEST_F(LoadTests, ShouldLoadManyIdsAsPostRequest)
	{
		std::vector<std::string> ids{};
		{
			auto session = test_suite_store->get().open_session();
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
			auto session = test_suite_store->get().open_session();

			auto users = session.load<infrastructure::entities::User>(ids.begin(), ids.end());
			auto user77 = users.at(std::string("users/77"));

			ASSERT_TRUE(user77);
			//may be rewritten to use GenerateEntityIdOnTheClient().trySetIdentity(entity, id);
			ASSERT_EQ(std::string("Person 77"), user77->name);
		}
	}

	//TODO implement
	TEST_F(LoadTests, LoadStartsWith)
	{
		SUCCEED();
	}
}
