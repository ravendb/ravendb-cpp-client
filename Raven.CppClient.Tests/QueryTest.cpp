#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"
#include "MaintenanceOperationExecutor.h"
#include "GetCollectionStatisticsOperation.h"

namespace query_test
{
	class UsersByName : public ravendb::client::documents::indexes::AbstractIndexCreationTask
	{
	public:
		~UsersByName() override = default;
		UsersByName()
		{
			SET_DEFAULT_INDEX_NAME;

			map = R"(
			from c in docs.Users
			select new
			{
				c.Name,
				count = 1
			})";

			reduce = R"(
			from result in results
			group result by result.Name
				into g
			select new
			{
				Name = g.Key,
				count = g.Sum(x => x.count)
			})";
		}
	};

	void add_users(std::shared_ptr<ravendb::client::documents::IDocumentStore> store)
	{
		auto session = store->open_session();

		auto user1 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user1->name = "John";
		user1->age = 3;
		session.store(user1);

		auto user2 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user2->name = "John";
		user2->age = 5;
		session.store(user2);

		auto user3 = std::make_shared<ravendb::client::tests::infrastructure::entities::User>();
		user3->name = "Tarzan";
		user3->age = 2;
		session.store(user3);

		session.store(user1, "users/1");
		session.store(user2, "users/2");
		session.store(user3, "users/3");
		session.save_changes();

		store->execute_index(std::make_shared<UsersByName>());
		ravendb::client::tests::driver::RavenTestDriver::wait_for_indexing(store);
	}

	struct ReduceResult
	{
		int32_t count{};
		std::string name{};
		int32_t age{};
	};

	void from_json(const nlohmann::json& j, ReduceResult& rr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "count", rr.count);
		get_val_from_json(j, "Name", rr.name);
		get_val_from_json(j, "Age", rr.age);
	}

	void to_json(nlohmann::json& j, const ReduceResult& rr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "count", rr.count);
		set_val_to_json(j, "Name", rr.name);
		set_val_to_json(j, "Age", rr.age);
	}

	struct UserProjection
	{
		std::string id{};
		std::string name{};
	};

	void from_json(const nlohmann::json& j, UserProjection& up)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "id", up.id);
		get_val_from_json(j, "Name", up.name);
	}

	void to_json(nlohmann::json& j, const UserProjection& up)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "id", up.id);
		set_val_to_json(j, "Name", up.name);
	}
}

namespace ravendb::client::tests::client
{
	class QueryTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
			REGISTER_ID_PROPERTY_FOR(query_test::UserProjection, id);
		}
	};

	TEST_F(QueryTest, QuerySimple)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->name = "John";

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "Jane";

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "Tarzan";

		session.store(user1, "users/1");
		session.store(user2, "users/2");
		session.store(user3, "users/3");
		session.save_changes();

		auto query_result = session.advanced().document_query<infrastructure::entities::User>(
			{}, "Users", false)
			->to_list();

		ASSERT_EQ(3, query_result.size());

		auto names = std::unordered_set<std::string>({"John", "Jane", "Tarzan"});
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

		auto session = store->open_session();

		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->name = "John";

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "Jane";

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "Tarzan";

		session.store(user1, "users/1");
		session.store(user2, "users/2");
		session.store(user3, "users/3");
		session.save_changes();

		auto lazy_result1 = session.query<infrastructure::entities::User>()
			->lazily();

		auto lazy_result2 = session.advanced().raw_query<infrastructure::entities::User>("from users")
			->lazily();

		auto query_result1 = lazy_result1.get_value();

		ASSERT_EQ(3, query_result1.size());

		auto query_result2 = lazy_result2.get_value();

		ASSERT_EQ(3, query_result2.size());
	}

	TEST_F(QueryTest, CollectionsStats)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1);

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexey";
			session.store(user2);

			session.save_changes();
		}

		auto stats = store->maintenance()->send(documents::operations::indexes::GetCollectionStatisticsOperation());

		ASSERT_EQ(3, stats->count_of_documents);
		ASSERT_EQ(2, stats->collections.at("Users"));
	}

	TEST_F(QueryTest, QueryWithWhereClause)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->name = "John";

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "Jane";

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "Tarzan";

		session.store(user1, "users/1");
		session.store(user2, "users/2");
		session.store(user3, "users/3");
		session.save_changes();

		auto query_result = session.query<infrastructure::entities::User>(documents::queries::Query::collection("users"))
			->where_starts_with("Name", std::string("J"))
			->to_list();

		auto query_result2 = session.query<infrastructure::entities::User>(documents::queries::Query::collection("users"))
			->where_equals("Name", "Tarzan")
			->to_list();

		auto query_result3 = session.query<infrastructure::entities::User>(documents::queries::Query::collection("users"))
			->where_ends_with("Name", 'n')
			->to_list();

		ASSERT_EQ(2, query_result.size());
		ASSERT_EQ(1, query_result2.size());
		ASSERT_EQ(2, query_result3.size());
	}

	TEST_F(QueryTest, QueryMapReduceWithCount)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto results = session.query<infrastructure::entities::User>()
			->group_by({ "Name" })
			->select_key()
			->select_count()
			->order_by_descending("count")
			->of_type<query_test::ReduceResult>()
			->to_list();

		ASSERT_EQ(2, results[0]->count);
		ASSERT_EQ("John", results[0]->name);

		ASSERT_EQ(1, results[1]->count);
		ASSERT_EQ("Tarzan", results[1]->name);
	}

	TEST_F(QueryTest, QueryMapReduceWithSum)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto results = session.query<infrastructure::entities::User>()
			->group_by({ "Name" })
			->select_key()
			->select_sum({documents::session::GroupByField("Age")})
			->order_by_descending("Age")
			->of_type<query_test::ReduceResult>()
			->to_list();

		ASSERT_EQ(8, results[0]->age);
		ASSERT_EQ("John", results[0]->name);

		ASSERT_EQ(2, results[1]->age);
		ASSERT_EQ("Tarzan", results[1]->name);
	}

	TEST_F(QueryTest, QueryMapReduceIndex)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto results = session.query<query_test::ReduceResult>(documents::queries::Query::index("UsersByName"))
			->order_by_descending("count")
			->to_list();

		ASSERT_EQ(2, results[0]->count);
		ASSERT_EQ("John", results[0]->name);

		ASSERT_EQ(1, results[1]->count);
		ASSERT_EQ("Tarzan", results[1]->name);
	}

	TEST_F(QueryTest, QuerySingleProperty)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto ages = session.query<infrastructure::entities::User>()
			->add_order("Age", true, documents::session::OrderingType::LONG)
			->select_fields<int32_t>({ "Age" })
			->to_list();

		ASSERT_EQ(3, ages.size());

		ASSERT_EQ(5, *ages[0]);
		ASSERT_EQ(3, *ages[1]);
		ASSERT_EQ(2, *ages[2]);
	}

	TEST_F(QueryTest, QueryWithSelect)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users_age = session.query<infrastructure::entities::User>()
			->select_fields<infrastructure::entities::User>({ "Age" })
			->to_list();

		ASSERT_EQ(3, users_age.size());

		for(const auto& user : users_age)
		{
			ASSERT_GT(user->age, 0);
			ASSERT_FALSE(user->id.empty());
		}
	}

	TEST_F(QueryTest, QueryWithWhereIn)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_in<std::string>("Name", {"Tarzan", "NoSuch"})
			->to_list();

		ASSERT_EQ(1, users.size());
	}

	TEST_F(QueryTest, QueryWithWhereBetween)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_between("Age", 4, 5)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("John", users[0]->name);
	}

	TEST_F(QueryTest, QueryWithWhereLessThan)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_less_than("Age", 3)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Tarzan", users[0]->name);
	}

	TEST_F(QueryTest, QueryWithWhereLessThanOrEqual)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_less_than_or_equal("Age", 3)
			->to_list();

		ASSERT_EQ(2, users.size());
	}

	TEST_F(QueryTest, QueryWithWhereGreaterThan)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_greater_than("Age", 3)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("John", users[0]->name);
	}

	TEST_F(QueryTest, QueryWithWhereGreaterThanOrEqual)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_greater_than_or_equal("Age", 3)
			->to_list();

		ASSERT_EQ(2, users.size());
	}

	TEST_F(QueryTest, QueryWithProjection)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto projections = session.query<infrastructure::entities::User>()
			->select_fields<query_test::UserProjection>()
			->to_list();

		ASSERT_EQ(3, projections.size());

		for(const auto& projection : projections)
		{
			ASSERT_FALSE(projection->id.empty());
			ASSERT_FALSE(projection->name.empty());
		}
	}

	TEST_F(QueryTest, QueryWithProjection2)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto projections = session.query<infrastructure::entities::User>()
			->select_fields<query_test::UserProjection>({"LastName"})
			->to_list();

		ASSERT_EQ(3, projections.size());

		for (const auto& projection : projections)
		{
			ASSERT_FALSE(projection->id.empty());
			ASSERT_TRUE(projection->name.empty());// we didn't specify this field in mapping
		}
	}

	TEST_F(QueryTest, QueryDistinct)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto unique_names = session.query<infrastructure::entities::User>()
			->select_fields<std::string>({ "Name" })
			->distinct()
			->to_list();

		ASSERT_EQ(2, unique_names.size());

		const auto unique_names_contains = [&](const std::string& str)->bool
		{
			return std::find_if(unique_names.begin(), unique_names.end(), [&](const auto& s)
			{
				return str == *s;
			}) != unique_names.end();
		};

		ASSERT_TRUE(unique_names_contains("Tarzan"));
		ASSERT_TRUE(unique_names_contains("John"));
	}

	TEST_F(QueryTest, QuerySearchWithOr)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Mickey";
		session.store(user);
		session.save_changes();

		auto names = session.query<infrastructure::entities::User>()
			->search("Name", "Tarzan John", documents::queries::SearchOperator::OR)
			->to_list();

		ASSERT_EQ(3, names.size());
	}

	TEST_F(QueryTest, QueryNoTracking)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->no_tracking()
			->to_list();

		ASSERT_EQ(3, users.size());

		for(const auto& user : users)
		{
			ASSERT_FALSE(session.advanced().is_loaded(user->id));
		}
	}

	TEST_F(QueryTest, QuerySkipTake)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->order_by("Name")
			->skip(2)
			->take(1)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Tarzan", users[0]->name);
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
			->add_parameter("p0", 5)
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("John", users[0]->name);
	}

	TEST_F(QueryTest, QueryLucene)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_Lucene("Name", "Tarzan")
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Tarzan", users[0]->name);
	}






	TEST_F(QueryTest, QueryParameters)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::add_users(store);

		auto session = store->open_session();

		ASSERT_EQ(1,
			session.advanced().raw_query<infrastructure::entities::User>("from Users where Name = $name")
			->add_parameter("name", "Tarzan")
			->count());
	}
}