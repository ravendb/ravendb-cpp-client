#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "LazySessionOperations.h"
#include "Company.h"
#include "User.h"

namespace ravendb::client::tests::client::lazy
{
	class LazyTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(LazyTest, CanLazilyLoadEntity)
	{
		{
			auto session = test_suite_store->get().open_session();
			for (auto i = 0; i <= 6; ++i)
			{
				auto company = std::make_shared<infrastructure::entities::Company>();
				std::string id = "companies/" + std::to_string(i);
				company->id = id;
				session.store(company, id);
			}
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

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
		{
			auto session = test_suite_store->get().open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->id = "companies/1";
			session.store(company1, "companies/1");

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->id = "companies/2";
			session.store(company2, "companies/2");

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

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
		{
			auto session = test_suite_store->get().open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			user->id = "users/1";
			session.store(user, user->id);

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

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

	//TODO implement
	//TEST_F(LazyTest, CanUseCacheWhenLazyLoading)
	//{}

	TEST_F(LazyTest, DontLazyLoadAlreadyLoadedValues)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			user1->id = "users/1";
			session.store(user1, user1->id);

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Boris";
			user2->id = "users/2";
			session.store(user2, user2->id);

			auto user3 = std::make_shared<infrastructure::entities::User>();
			user3->name = "Dmitriy";
			user3->id = "users/3";
			session.store(user3, user3->id);

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

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
}
