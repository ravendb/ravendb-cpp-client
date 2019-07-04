#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "LazySessionOperations.h"
#include "Order.h"

namespace caching_of_document_include
{
	struct User
	{
		std::string id{};
		std::string name{};
		std::string partnerId{};
		std::string email{};
		std::vector<std::string> tags{};
		int32_t age{};
		bool active{};
	};

	inline void to_json(nlohmann::json& j, const User& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "id", u.id);
		set_val_to_json(j, "name", u.name);
		set_val_to_json(j, "partnerId", u.partnerId);
		set_val_to_json(j, "email", u.email);
		set_val_to_json(j, "tags", u.tags);
		set_val_to_json(j, "age", u.age);
		set_val_to_json(j, "active", u.active);
	}

	inline void from_json(const nlohmann::json& j, User& u)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "id", u.id);
		get_val_from_json(j, "name", u.name);
		get_val_from_json(j, "partnerId", u.partnerId);
		get_val_from_json(j, "email", u.email);
		get_val_from_json(j, "tags", u.tags);
		get_val_from_json(j, "age", u.age);
		get_val_from_json(j, "active", u.active);
	}

	struct Product
	{
		std::string id{};
		std::string name{};
	};

	inline void to_json(nlohmann::json& j, const Product& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "id", p.id);
		set_val_to_json(j, "name", p.name);
	}

	inline void from_json(const nlohmann::json& j, Product& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "id", p.id);
		get_val_from_json(j, "name", p.name);
	}
}

namespace ravendb::client::tests::bugs::caching
{
	class CachingOfDocumentInclude : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(caching_of_document_include::User, id);
			REGISTER_ID_PROPERTY_FOR(caching_of_document_include::Product, id); 
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::Order, id);
		}
	};

	TEST_F(CachingOfDocumentInclude, CanCacheDocumentWithIncludes)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<caching_of_document_include::User>();
			user->name = "Alexander";
			user->id = "users/1";
			session.store(user);

			auto partner = std::make_shared<caching_of_document_include::User>();
			partner->partnerId = "users/1";
			partner->id = "users/2";
			session.store(partner);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			session.include("partnerId")
				.load<caching_of_document_include::User>("users/2");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			session.include("partnerId").load<caching_of_document_include::User>("users/2");

			ASSERT_EQ(1, session.advanced().get_request_executor()->get_cache()->get_number_of_items());
		}
	}

	TEST_F(CachingOfDocumentInclude, CanAvoidUsingServerForLoadWithIncludeIfEverythingIsInSessionCacheAsync)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<caching_of_document_include::User>();
			user->name = "Alexander";
			user->id = "users/1";
			session.store(user);

			auto partner = std::make_shared<caching_of_document_include::User>();
			partner->partnerId = "users/1";
			partner->id = "users/2";
			session.store(partner);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<caching_of_document_include::User>("users/2");
			session.load<caching_of_document_include::User>(user->partnerId);

			auto old = session.advanced().get_number_of_requests();
			auto new_user = session.include("partnerId")
				.load<caching_of_document_include::User>("users/2");

			ASSERT_EQ(old, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(CachingOfDocumentInclude, CanAvoidUsingServerForLoadWithIncludeIfEverythingIsInSessionCacheLazy)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<caching_of_document_include::User>();
			user->name = "Alexander";
			user->id = "users/1";
			session.store(user);

			auto partner = std::make_shared<caching_of_document_include::User>();
			partner->partnerId = "users/1";
			partner->id = "users/2";
			session.store(partner);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			session.advanced().lazily().load<caching_of_document_include::User>("users/2");
			session.advanced().lazily().load<caching_of_document_include::User>("users/1");
			session.advanced().eagerly().execute_all_pending_lazy_operations();

			auto old = session.advanced().get_number_of_requests();

			auto result1 = session.advanced().lazily()
				.include("partnerId")
				.load<caching_of_document_include::User>("users/2");

			ASSERT_TRUE(result1.get_value());
			ASSERT_EQ(old, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(CachingOfDocumentInclude, CanAvoidUsingServerForLoadWithIncludeIfEverythingIsInSessionCache)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<caching_of_document_include::User>();
			user->name = "Alexander";
			user->id = "users/1";
			session.store(user);

			auto partner = std::make_shared<caching_of_document_include::User>();
			partner->partnerId = "users/1";
			partner->id = "users/2";
			session.store(partner);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<caching_of_document_include::User>("users/2");

			session.load<caching_of_document_include::User>(user->partnerId);

			auto old = session.advanced().get_number_of_requests();

			auto res = session.include("partnerId")
				.load<caching_of_document_include::User>("users/2");

			ASSERT_EQ(old, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(CachingOfDocumentInclude, CanAvoidUsingServerForMultiloadWithIncludeIfEverythingIsInSessionCache)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			const auto store_user = [&](std::string name, int32_t number)
			{
				auto user = std::make_shared<caching_of_document_include::User>();
				user->name = std::move(name);
				user->id = "users/" + std::to_string(number);
				session.store(user);
			};

			store_user("Alexander", 1);
			store_user("Alexey", 2);
			store_user("Boris", 3);
			store_user("Igor", 4);
			store_user("Vitaliy", 5);

			auto with_partner = std::make_shared<caching_of_document_include::User>();
			with_partner->id = "users/6";
			with_partner->partnerId = "users/1";
			session.store(with_partner);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto u2 = session.load<caching_of_document_include::User>("users/2");
			auto u6 = session.load<caching_of_document_include::User>("users/6");

			std::vector<std::string> inp{ "users/1","users/2","users/3","users/4","users/5","users/6" };
			auto u4 = session.load<caching_of_document_include::User>(inp);

			session.load<caching_of_document_include::User>(u6->partnerId);

			int old = session.advanced().get_number_of_requests();

			auto res = session.include("partnerId")
				.load<caching_of_document_include::User>({ "users/2","users/3", "users/6" });

			ASSERT_EQ(old, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(CachingOfDocumentInclude, CanIncludNestedPaths)
	{
		auto store = get_document_store(TEST_NAME);

		auto order = std::make_shared<infrastructure::entities::Order>();

		auto order_line1 = infrastructure::entities::OrderLine();
		order_line1.product = "products/1";
		order_line1.productName = "phone";

		auto order_line2 = infrastructure::entities::OrderLine();
		order_line2.product = "products/2";
		order_line2.productName = "mouse";

		order->lines = { std::move(order_line1), std::move(order_line2) };

		auto product1 = std::make_shared<caching_of_document_include::Product>();
		product1->id = "products/1";
		product1->name = "phone";

		auto product2 = std::make_shared<caching_of_document_include::Product>();
		product2->id = "products/2";
		product2->name = "mouse";

		{
			auto session = store->open_session();

			session.store(order, "orders/1");
			session.store(product1);
			session.store(product2);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			ASSERT_EQ(0, session.advanced().get_number_of_requests());

			auto orders = session.query<infrastructure::entities::Order>()
				->include([](auto builder){builder->include_documents("lines[].product");})
				->to_list();

			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			auto product = session.load<caching_of_document_include::Product>(orders.at(0)->lines.at(0).product);

			ASSERT_EQ(1, session.advanced().get_number_of_requests());
		}
		//TODO
		//there is another, currently not working, part on the java client test -> issue opened RDBC-345 
	}
}