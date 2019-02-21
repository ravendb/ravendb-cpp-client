#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "RequestExecutor.h"
#include "User.h"
#include "IndexDefinition.h"
#include "PutIndexesOperation.h"

namespace query_test
{
	void add_users(ravendb::client::documents::DocumentStore& store)
	{
		auto session = store.open_session();

		auto user1 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user1->name = "Vlad";
		user1->age = 30;
		session.store(user1, "users/1");

		auto user2 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user2->name = "Vlad";
		user2->age = 50;
		session.store(user2, "users/2");

		auto user3 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user3->name = "Tuzik";
		user3->age = 2;
		session.store(user3, "users/3");

		session.save_changes();

		//TODO
		//store.executeIndex(new UsersByName());
		//waitForIndexing(store);

		ravendb::client::documents::indexes::IndexDefinition index{};
		index.name = "UsersByName";
		index.maps = { R"(
			from c in docs.Users
			select new
			{
				c.name,
				count = 1
			})" };

		auto op = ravendb::client::documents::operations::indexes::PutIndexesOperation({ index });
		store.get_request_executor()->execute(op.get_command({}));

		//waiting for indexing TODO something better
		std::this_thread::sleep_for(std::chrono::duration<int>(3));
	}
}

namespace ravendb::client::tests::client
{
	class QueryTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(QueryTest, RawQuerySkipTake)
	{
		query_test::add_users(test_suite_store->get());

		auto session = test_suite_store->get().open_session();

		auto users = session.advanced().raw_query("from users")
			->skip(2)
			->take(1)
			->to_list<infrastructure::entities::User>();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Tuzik", users[0]->name);
	}

	TEST_F(QueryTest, ParametersInRawQuery)
	{
		query_test::add_users(test_suite_store->get());

		auto session = test_suite_store->get().open_session();

		auto users = session.advanced().raw_query("from Users where Age = $p0")
			->add_parameter("p0", 50)
			->to_list<infrastructure::entities::User>();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Vlad", users[0]->name);
	}

	TEST_F(QueryTest, QueryParameters)
	{
		query_test::add_users(test_suite_store->get());

		auto session = test_suite_store->get().open_session();

		ASSERT_EQ(1, 
			session.advanced().raw_query("from Users where Name = $name")
				->add_parameter("name", "Tuzik")
				->count());
	}
}