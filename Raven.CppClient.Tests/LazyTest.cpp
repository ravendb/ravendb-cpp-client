#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "LazySessionOperations.h"
#include "Company.h"
#include "User.h"
#include "Employee.h"
#include "Order.h"
#include "EntityIdHelperUtil.h"

namespace ravendb::client::tests::client::lazy
{
	class LazyTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::Company, id);
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::Employee, id);
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::Order, id);
		}
	};

	TEST_F(LazyTest, CanLazilyLoadEntity)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			for (auto i = 0; i <= 6; ++i)
			{
				auto company = std::make_shared<infrastructure::entities::Company>();
				std::string id = "companies/" + std::to_string(i);
				session.store(company, id);
			}
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto lazy_company = session.advanced().lazily().load<infrastructure::entities::Company>("companies/1");
			ASSERT_FALSE(lazy_company.is_value_created());

			auto company = lazy_company.get_value();
			ASSERT_EQ("companies/1", company->id);

			auto lazy_companies = session.advanced().lazily().load<infrastructure::entities::Company>
				(std::vector<std::string>{ "companies/1", "companies/2" });
			ASSERT_FALSE(lazy_companies.is_value_created());

			auto companies = lazy_companies.get_value();
			ASSERT_EQ(2, companies.size());

			auto company1 = companies.at("companies/1");
			auto company2 = companies.at("companies/2");

			ASSERT_TRUE(company1);
			ASSERT_TRUE(company2);

			ASSERT_EQ("companies/1", company1->id);
			ASSERT_EQ("companies/2", company2->id);

			lazy_company = session.advanced().lazily().load<infrastructure::entities::Company>("companies/3");
			ASSERT_FALSE(lazy_company.is_value_created());

			company = lazy_company.get_value();
			ASSERT_EQ("companies/3", company->id);

			auto load = session.advanced().lazily().load<infrastructure::entities::Company>
				(std::vector<std::string>{"no_such_1", "no_such_2"});

			auto missing_items = load.get_value();

			ASSERT_FALSE(missing_items.at("no_such_1"));
			ASSERT_FALSE(missing_items.at("no_such_2"));
		}
	}

	TEST_F(LazyTest, CanExecuteAllPendingLazyOperations)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->id = "companies/1";
			session.store(company1, "companies/1");

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->id = "companies/2";
			session.store(company2, "companies/2");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::shared_ptr<infrastructure::entities::Company> company1_ptr{};
			std::shared_ptr<infrastructure::entities::Company> company2_ptr{};

			session.advanced().lazily().load<infrastructure::entities::Company>("companies/1",
				[&](const std::shared_ptr<infrastructure::entities::Company>& company)
			{
				company1_ptr = company;
			});

			session.advanced().lazily().load<infrastructure::entities::Company>("companies/2",
				[&](const std::shared_ptr<infrastructure::entities::Company>& company)
			{
				company2_ptr = company;
			});

			ASSERT_FALSE(company1_ptr);
			ASSERT_FALSE(company2_ptr);

			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_TRUE(company1_ptr);
			ASSERT_TRUE(company2_ptr);

			ASSERT_EQ("companies/1", company1_ptr->id);
			ASSERT_EQ("companies/2", company2_ptr->id);
		}
	}

	TEST_F(LazyTest, WithQueuedActions_Load)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::shared_ptr<infrastructure::entities::User> user_ptr{};

			session.advanced().lazily().load<infrastructure::entities::User>("users/1", 
				[&](const std::shared_ptr<infrastructure::entities::User>& user_param)
			{
				user_ptr = user_param;
			});

			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_TRUE(user_ptr);
		}
	}

	TEST_F(LazyTest, CanUseCacheWhenLazyLoading)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto lazy_loaded = session.advanced().lazily().load<infrastructure::entities::User>("users/1");

			ASSERT_EQ(0, session.advanced().get_number_of_requests());

			auto loaded = session.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ("Alexander", loaded->name);

			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			ASSERT_EQ("Alexander", lazy_loaded.get_value()->name);

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
		}

	}

	TEST_F(LazyTest, DontLazyLoadAlreadyLoadedValues)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1, "users/1");

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Boris";
			session.store(user2, "users/2");

			auto user3 = std::make_shared<infrastructure::entities::User>();
			user3->name = "Dmitriy";
			session.store(user3, "users/3");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto lazy_load = session.advanced().lazily().load<infrastructure::entities::User>
				(std::vector<std::string>{"users/2", "users/3"});
			session.advanced().lazily().load<infrastructure::entities::User>
				(std::vector<std::string>{"users/1", "users/3"});

			session.load<infrastructure::entities::User>("users/2");
			session.load<infrastructure::entities::User>("users/3");

			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_TRUE(session.advanced().is_loaded("users/1"));

			auto users = lazy_load.get_value();
			ASSERT_EQ(2, users.size());

			auto old_requests_count = session.advanced().get_number_of_requests();
			lazy_load = session.advanced().lazily().load<infrastructure::entities::User>
				(std::vector<std::string>{"users/3"});
			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_EQ(old_requests_count, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(LazyTest, LazyLoadWithIncludes)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto emp1 = std::make_shared<infrastructure::entities::Employee>();
			emp1->firstName = "Boris";
			emp1->id = "employees/1";
			session.store(emp1);

			auto emp2 = std::make_shared<infrastructure::entities::Employee>();
			emp2->firstName = "Vitaliy";
			emp2->id = "employees/2";
			session.store(emp2);

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "Pear";
			company->id = "companies/1";
			company->employeesIds = { "employees/1" ,"employees/2" };
			session.store(company);

			auto order1 = std::make_shared<infrastructure::entities::Order>();
			order1->id = "orders/1";
			order1->company = "companies/1";
			order1->employee = "employees/1";
			session.store(order1);

			auto order2 = std::make_shared<infrastructure::entities::Order>();
			order2->id = "orders/2";
			order2->company = "companies/1";
			order2->employee = "employees/2";
			session.store(order2);

			session.save_changes();			
		}
		{
			auto session = store->open_session();

			auto lazy_order = session.advanced().lazily()
				.include("employee")
				.include("company")
				.load<infrastructure::entities::Order>("orders/1");

			ASSERT_EQ(0, session.advanced().get_number_of_requests());

			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
			ASSERT_TRUE(session.advanced().is_loaded("orders/1"));
			ASSERT_TRUE(session.advanced().is_loaded("employees/1"));
			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));

			ASSERT_FALSE(lazy_order.is_value_created());
			ASSERT_EQ("orders/1", lazy_order.get_value()->id);
			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			auto company = session.load<infrastructure::entities::Company>("companies/1");
			ASSERT_EQ("Pear", company->name);
			ASSERT_EQ(1, session.advanced().get_number_of_requests());			
		}
		{
			auto session = store->open_session();
			std::vector<std::string> ids = { "orders/1", "orders/2" };

			auto lazy_orders = session.advanced().lazily()
				.include("employee")
				.load<infrastructure::entities::Order>(ids.begin(), ids.end());

			ASSERT_EQ(0, session.advanced().get_number_of_requests());

			session.advanced().eagerly().execute_all_pending_lazy_operations();

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
			ASSERT_TRUE(session.advanced().is_loaded("employees/1"));
			ASSERT_TRUE(session.advanced().is_loaded("employees/2"));
			ASSERT_TRUE(session.advanced().is_loaded("orders/1"));

			ASSERT_FALSE(lazy_orders.is_value_created());
			ASSERT_EQ(2, lazy_orders.get_value().size());
			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			auto empl = session.load<infrastructure::entities::Employee>("employees/1");
			ASSERT_EQ("Boris", empl->firstName);
			ASSERT_EQ(1, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(LazyTest, CanRequestServerOnceForAllLazyOperations)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Vanya";
			user1->id = "users/1";
			session.store(user1, user1->id);

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Vasya";
			user2->id = "users/2";
			session.store(user2, user2->id);

			auto empl = std::make_shared<infrastructure::entities::Employee>();
			empl->firstName = "Misha";
			session.store(empl, "employees/1");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto lazy_load = session.advanced()
				.lazily()
				.load<infrastructure::entities::Employee>("employees/1");

			auto lazy_query = session.query<infrastructure::entities::User>()
				->lazily();

			auto empl = lazy_load.get_value();

			ASSERT_EQ("Misha", empl->firstName);

			auto names = std::set<std::string>{ lazy_query.get_value()[0]->name, lazy_query.get_value()[1]->name };

			ASSERT_TRUE(names.find("Vasya") != names.end());
			ASSERT_TRUE(names.find("Vanya") != names.end());

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
		}
	}
}
