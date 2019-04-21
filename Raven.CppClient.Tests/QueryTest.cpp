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
			SET_DEFAULT_INDEX_NAME();

			map = R"(
			from c in docs.Users
			select new
			{
				c.name,
				count = 1
			})";

			reduce = R"(
			from result in results
			group result by result.name
				into g
			select new
			{
				name = g.Key,
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
		get_val_from_json(j, "name", rr.name);
		get_val_from_json(j, "age", rr.age);
	}

	void to_json(nlohmann::json& j, const ReduceResult& rr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "count", rr.count);
		set_val_to_json(j, "name", rr.name);
		set_val_to_json(j, "age", rr.age);
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
		get_val_from_json(j, "name", up.name);
	}

	void to_json(nlohmann::json& j, const UserProjection& up)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "id", up.id);
		set_val_to_json(j, "name", up.name);
	}

	struct Dog
	{
		std::string id{};
		std::string name{};
		std::string breed{};
		std::string color{};
		int32_t age{};
		bool is_vaccinated{};
	};

	void from_json(const nlohmann::json& j, Dog& d)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", d.name);
		get_val_from_json(j, "breed", d.breed);
		get_val_from_json(j, "color", d.color);
		get_val_from_json(j, "age", d.age);
		get_val_from_json(j, "vaccinated", d.is_vaccinated);
	}

	void to_json(nlohmann::json& j, const Dog& d)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", d.name);
		set_val_to_json(j, "breed", d.breed);
		set_val_to_json(j, "color", d.color);
		set_val_to_json(j, "age", d.age);
		set_val_to_json(j, "vaccinated", d.is_vaccinated);
	}

	class DogsIndex : public ravendb::client::documents::indexes::AbstractIndexCreationTask
	{
	public:
		~DogsIndex() override = default;
		DogsIndex()
		{
			SET_DEFAULT_INDEX_NAME();

			map = R"(
			from dog in docs.dogs
			select new 
			{
				dog.name,
				dog.age,
				dog.vaccinated
			})";
		}
	};

	struct DogsIndexResult
	{
		std::string name{};
		int32_t age{};
		bool is_vaccinated{};
	};

	void from_json(const nlohmann::json& j, DogsIndexResult& dir)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", dir.name);
		get_val_from_json(j, "age", dir.age);
		get_val_from_json(j, "vaccinated", dir.is_vaccinated);
	}

	void to_json(nlohmann::json& j, const DogsIndexResult& dir)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", dir.name);
		set_val_to_json(j, "age", dir.age);
		set_val_to_json(j, "vaccinated", dir.is_vaccinated);
	}

	void create_dogs(ravendb::client::documents::session::DocumentSession& session) 
	{
		auto dog1 = std::make_shared<Dog>();
		dog1->name = "Snoopy";
		dog1->breed = "Beagle";
		dog1->color = "White";
		dog1->age = 6;
		dog1->is_vaccinated = true;

		session.store(dog1, "dogs/1");

		auto dog2 = std::make_shared<Dog>();
		dog2->name = "Brian";
		dog2->breed = "Labrador";
		dog2->color = "White";
		dog2->age = 12;
		dog2->is_vaccinated = false;

		session.store(dog2, "dogs/2");

		auto dog3 = std::make_shared<Dog>();
		dog3->name = "Django";
		dog3->breed = "Jack Russel";
		dog3->color = "Black";
		dog3->age = 3;
		dog3->is_vaccinated = true;

		session.store(dog3, "dogs/3");

		auto dog4 = std::make_shared<Dog>();
		dog4->name = "Beethoven";
		dog4->breed = "St. Bernard";
		dog4->color = "Brown";
		dog4->age = 1;
		dog4->is_vaccinated = false;

		session.store(dog4, "dogs/4");

		auto dog5 = std::make_shared<Dog>();
		dog5->name = "Scooby Doo";
		dog5->breed = "Great Dane";
		dog5->color = "Brown";
		dog5->age = 0;
		dog5->is_vaccinated = false;

		session.store(dog5, "dogs/5");

		auto dog6 = std::make_shared<Dog>();
		dog6->name = "Old Yeller";
		dog6->breed = "Black Mouth Cur";
		dog6->color = "White";
		dog6->age = 2;
		dog6->is_vaccinated = true;

		session.store(dog6, "dogs/6");

		auto dog7 = std::make_shared<Dog>();
		dog7->name = "Benji";
		dog7->breed = "Mixed";
		dog7->color = "White";
		dog7->age = 0;
		dog7->is_vaccinated = false;

		session.store(dog7, "dogs/7");

		auto dog8 = std::make_shared<Dog>();
		dog8->name = "Lassie";
		dog8->breed = "Collie";
		dog8->color = "Brown";
		dog8->age = 6;
		dog8->is_vaccinated = true;

		session.store(dog8, "dogs/8");
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

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
			REGISTER_ID_PROPERTY_FOR(query_test::UserProjection, id);
			REGISTER_ID_PROPERTY_FOR(query_test::Dog, id);
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
			->where_starts_with("name", std::string("J"))
			->to_list();

		auto query_result2 = session.query<infrastructure::entities::User>(documents::queries::Query::collection("users"))
			->where_equals("name", "Tarzan")
			->to_list();

		auto query_result3 = session.query<infrastructure::entities::User>(documents::queries::Query::collection("users"))
			->where_ends_with("name", 'n')
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
			->group_by({ "name" })
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
			->group_by({ "name" })
			->select_key()
			->select_sum({documents::session::GroupByField("age")})
			->order_by_descending("age")
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
			->add_order("age", true, documents::session::OrderingType::LONG)
			->select_fields<int32_t>({ "age" })
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
			->select_fields<infrastructure::entities::User>({ "age" })
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
			->where_in<std::string>("name", {"Tarzan", "NoSuch"})
			->to_list();

		ASSERT_EQ(1, users.size());
	}

	TEST_F(QueryTest, QueryWithWhereBetween)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_between("age", 4, 5)
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
			->where_less_than("age", 3)
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
			->where_less_than_or_equal("age", 3)
			->to_list();

		ASSERT_EQ(2, users.size());
	}

	TEST_F(QueryTest, QueryWithWhereGreaterThan)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_greater_than("age", 3)
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
			->where_greater_than_or_equal("age", 3)
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
			->select_fields<query_test::UserProjection>({"lastName"})
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
			->select_fields<std::string>({ "name" })
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
			->search("name", "Tarzan John", documents::queries::SearchOperator::OR)
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
			->order_by("name")
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

		auto users = session.advanced().raw_query<infrastructure::entities::User>("from Users where age = $p0")
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
			->where_lucene("name", "Tarzan")
			->to_list();

		ASSERT_EQ(1, users.size());
		ASSERT_EQ("Tarzan", users[0]->name);
	}

	TEST_F(QueryTest, QueryWhereExact)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_equals("name", "tarzan")
			->to_list();

		ASSERT_EQ(1, users.size());

		users = session.query<infrastructure::entities::User>()
			->where_equals("name", "tarzan", true)
			->to_list();

		ASSERT_EQ(0, users.size());// we queried for tarzan with exact

		users = session.query<infrastructure::entities::User>()
			->where_equals("name", "Tarzan", true)
			->to_list();

		ASSERT_EQ(1, users.size());// we queried for Tarzan with exact
	}

	TEST_F(QueryTest, QueryWhereNot)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->not_next()
			->where_equals("name", "tarzan")
			->to_list();

		ASSERT_EQ(2, users.size());

		users = session.query<infrastructure::entities::User>()
			->where_not_equals("name", "tarzan")
			->to_list();

		ASSERT_EQ(2, users.size());

		users = session.query<infrastructure::entities::User>()
			->where_not_equals("name", "Tarzan", true)
			->to_list();

		ASSERT_EQ(2, users.size());
	}

	//TODO
	//public void queryWithDuration() throws Exception {

	TEST_F(QueryTest, QueryFirst)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto first = session.query<infrastructure::entities::User>()
			->first();

		ASSERT_TRUE(first);

		auto single = session.query<infrastructure::entities::User>()
			->where_equals("name", "Tarzan")
			->single();

		ASSERT_TRUE(single);

		ASSERT_TRUE(first);

		try
		{
			session.query<infrastructure::entities::User>()
				->single();
		}
		catch (std::runtime_error& e)
		{
			SUCCEED();
			return;
		}
		FAIL();
	}

	TEST_F(QueryTest, QueryParameters)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		ASSERT_EQ(1,
			session.advanced().raw_query<infrastructure::entities::User>("from Users where name = $name")
			->add_parameter("name", "Tarzan")
			->count());
	}

	TEST_F(QueryTest, QueryRandomOrder)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->random_ordering()
			->to_list();

		ASSERT_EQ(3, users.size());

		users = session.query<infrastructure::entities::User>()
			->random_ordering("123")
			->to_list();

		ASSERT_EQ(3, users.size());
	}

	TEST_F(QueryTest, QueryWhereExists)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_exists("name")
			->to_list();

		ASSERT_EQ(3, users.size());

		users = session.query<infrastructure::entities::User>()
			->where_exists("name")
			->and_also()
			->not_next()
			->where_exists("no_such_field")
			->to_list();

		ASSERT_EQ(3, users.size());
	}

	TEST_F(QueryTest, QueryWithBoost)
	{
		auto store = get_document_store(TEST_NAME);
		query_test::add_users(store);
		auto session = store->open_session();

		auto users = session.query<infrastructure::entities::User>()
			->where_equals("name", "Tarzan")
			->boost(5)
			->or_else()
			->where_equals("name", "John")
			->boost(2)
			->order_by_score()
			->to_list();

		ASSERT_EQ(3, users.size());
		ASSERT_EQ("Tarzan", users[0]->name);
		ASSERT_EQ("John", users[1]->name);
		ASSERT_EQ("John", users[2]->name);

		users = session.query<infrastructure::entities::User>()
			->where_equals("name", "Tarzan")
			->boost(2)
			->or_else()
			->where_equals("name", "John")
			->boost(5)
			->order_by_score()
			->to_list();

		ASSERT_EQ(3, users.size());
		ASSERT_EQ("John", users[0]->name);
		ASSERT_EQ("John", users[1]->name);
		ASSERT_EQ("Tarzan", users[2]->name);
	}

	TEST_F(QueryTest, QueryWithCustomize)
	{
		auto store = get_document_store(TEST_NAME);

		query_test::DogsIndex().execute(store);

		{
			auto session = store->open_session();
			query_test::create_dogs(session);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto query_result = session.advanced()
				.document_query<query_test::DogsIndexResult>(query_test::DogsIndex().get_index_name(), {}, false)
				->wait_for_non_stale_results()
				->order_by("name", documents::session::OrderingType::ALPHA_NUMERIC)
				->where_greater_than("age", 2)
				->to_list();

			ASSERT_EQ(4, query_result.size());

			ASSERT_EQ("Brian", query_result[0]->name);
			ASSERT_EQ("Django", query_result[1]->name);
			ASSERT_EQ("Lassie", query_result[2]->name);
			ASSERT_EQ("Snoopy", query_result[3]->name);
		}
	}

	TEST_F(QueryTest, QueryLongRequest)
	{
		auto store = get_document_store(TEST_NAME);
		auto session = store->open_session();

		auto long_name = std::string(2048, 'x');
		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = long_name;
		session.store(user, "users/1");

		session.save_changes();

		auto query_result = session.advanced()
			.document_query<infrastructure::entities::User>({}, "Users", false)
			->where_equals("name", long_name)
			->to_list();

		ASSERT_EQ(1, query_result.size());
	}

	TEST_F(QueryTest, QueryByIndex)
	{
		auto store = get_document_store(TEST_NAME);
		
		query_test::DogsIndex().execute(store);

		{
			auto session = store->open_session();
			query_test::create_dogs(session);
			session.save_changes();

			wait_for_indexing(store, store->get_database());
		}
		{
			auto session = store->open_session();

			auto query_result = session.advanced()
				.document_query<query_test::DogsIndexResult>(query_test::DogsIndex().get_index_name(), {}, false)
				->where_greater_than("age", 2)
				->and_also()
				->where_equals("vaccinated", false)
				->to_list();

			ASSERT_EQ(1, query_result.size());
			ASSERT_EQ("Brian", query_result[0]->name);

			query_result = session.advanced()
				.document_query<query_test::DogsIndexResult>(query_test::DogsIndex().get_index_name(), {}, false)
				->where_less_than_or_equal("age", 2)
				->and_also()
				->where_equals("vaccinated", false)
				->to_list();

			ASSERT_EQ(3, query_result.size());

			std::set<std::string> names_from_query;
			std::transform(query_result.begin(), query_result.end(), std::inserter(names_from_query, names_from_query.end()),
				[](const auto& res)
			{
				return res->name;
			});

			const auto names_from_query_contains = [&](const std::string& name)
			{
				return names_from_query.find(name) != names_from_query.end();
			};

			ASSERT_TRUE(names_from_query_contains("Beethoven"));
			ASSERT_TRUE(names_from_query_contains("Scooby Doo"));
			ASSERT_TRUE(names_from_query_contains("Benji"));
		}
	}
}