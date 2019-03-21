#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "RequestExecutor.h"
#include "User.h"
#include "IndexDefinition.h"
#include "PutIndexesOperation.h"

namespace query_test
{
	void add_users(std::shared_ptr<ravendb::client::documents::DocumentStore> store)
	{
		auto session = store->open_session();

		auto user1 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user1->name = "Vlad";
		user1->age = 30;
		user1->id = "users/1";
		session.store(user1);

		auto user2 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user2->name = "Slava";
		user2->age = 50;
		user2->id = "users/2";
		session.store(user2);

		auto user3 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user3->name = "Tuzik";
		user3->age = 2;
		user3->id = "users/3";
		session.store(user3);

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
		store->get_request_executor()->execute(op.get_command(store->get_conventions()));

		ravendb::client::tests::driver::RavenTestDriver::wait_for_indexing(store);
	}
}

namespace ravendb::client::tests::client
{
	class QueryTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(QueryTest, QuerySimple)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		auto query_result = session.advanced().document_query<infrastructure::entities::User>(
			{}, "Users", false)
			->to_list();

		ASSERT_EQ(3, query_result.size());

		auto names = std::unordered_set<std::string>({"Vlad", "Slava", "Tuzik"});
		auto results = std::unordered_set<std::string>();
		std::transform(query_result.cbegin(), query_result.cend(), std::insert_iterator(results,results.end()),
			[](const auto& user)
		{
			return user->name;
		});

		ASSERT_EQ(names, results);
	}

	TEST_F(QueryTest, QueryLazily)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		auto lazy_result1 = session.query<infrastructure::entities::User>()
			->lazily();

		auto lazy_result2 = session.advanced().raw_query<infrastructure::entities::User>("from users")
			->lazily();

		auto query_result1 = lazy_result1.get_value();

		ASSERT_EQ(3, query_result1.size());
		ASSERT_EQ("Vlad", query_result1[0]->name);

		auto query_result2 = lazy_result2.get_value();

		ASSERT_EQ(3, query_result2.size());
		ASSERT_EQ("Vlad", query_result2[0]->name);
	}

	//TODO be more specific about the result, since the order is not guarantied
	TEST_F(QueryTest, QuerySkipTake)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		auto users1 = session.query<infrastructure::entities::User>()
			->to_list();

		auto users2 = session.query<infrastructure::entities::User>()
		//TODO original test uses order_by
			->skip(2)
			->take(1)
			->to_list();

		ASSERT_EQ(1, users2.size());
		ASSERT_EQ(users1[2]->name, users2[0]->name);
	}


	//TODO be more specific about the result, since the order is not guarantied
	TEST_F(QueryTest, RawQuerySkipTake)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		auto users1 = session.advanced().raw_query<infrastructure::entities::User>("from users")
			->to_list();

		auto users2 = session.advanced().raw_query<infrastructure::entities::User>("from users")
			->skip(2)
			->take(1)
			->to_list();

		ASSERT_EQ(1, users2.size());
		ASSERT_EQ(users1[2]->name, users2[0]->name);
	}

	TEST_F(QueryTest, ParametersInRawQuery)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		auto users = session.advanced().raw_query<infrastructure::entities::User>("from Users where Age = $p0")
			->add_parameter("p0", 30)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Vlad", users[0]->name);
	}

	TEST_F(QueryTest, QueryParameters)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		ASSERT_EQ(1,
			session.advanced().raw_query<infrastructure::entities::User>("from Users where Name = $name")
			->add_parameter("name", "Tuzik")
			->count());
	}
}