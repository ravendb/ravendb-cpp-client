#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "Order.h"
#include "AdvancedSessionOperations.h"

namespace RavenDB_8761
{
	struct ProductCount
	{
		std::string productName{};
		int32_t count{};
		std::string country{};
		int32_t quantity{};
		std::vector<std::string> products{};
		std::vector<int32_t> quantities{};
	};

	void from_json(const nlohmann::json& j, ProductCount& pc)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "productName", pc.productName);
		get_val_from_json(j, "count", pc.count);
		get_val_from_json(j, "country", pc.country);
		get_val_from_json(j, "quantity", pc.quantity);
		get_val_from_json(j, "products", pc.products);
		get_val_from_json(j, "quantities", pc.quantities);
		
	}

	void to_json(nlohmann::json& j, const ProductCount& pc)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "productName", pc.productName);
		set_val_to_json(j, "count", pc.count);
		set_val_to_json(j, "country", pc.country);
		set_val_to_json(j, "quantity", pc.quantity);
		set_val_to_json(j, "products", pc.products);
		set_val_to_json(j, "quantities", pc.quantities);
	}

	static void put_docs(std::shared_ptr<ravendb::client::documents::IDocumentStore> store)
	{
		auto session = store->open_session();

		auto order1 = std::make_shared<ravendb::client::tests::infrastructure::entities::Order>();
		
		auto order_line11 = ravendb::client::tests::infrastructure::entities::OrderLine();
		order_line11.product = "products/1";
		order_line11.quantity = 1;

		auto order_line12 = ravendb::client::tests::infrastructure::entities::OrderLine();
		order_line12.product = "products/2";
		order_line12.quantity = 2;

		order1->lines.push_back(std::move(order_line11));
		order1->lines.push_back(std::move(order_line12));

		auto address1 = ravendb::client::tests::infrastructure::entities::Address();
		address1.country = "USA";

		order1->shipTo = address1;

		session.store(order1);

		auto order_line21 = ravendb::client::tests::infrastructure::entities::OrderLine();
		order_line21.product = "products/2";
		order_line21.quantity = 3;

		auto address2 = ravendb::client::tests::infrastructure::entities::Address();
		address2.country = "USA";
		auto order2 = std::make_shared<ravendb::client::tests::infrastructure::entities::Order>();
		order2->lines.push_back(std::move(order_line21));
		order2->shipTo = address2;
		session.store(order2);

		session.save_changes();
	}
}

namespace ravendb::client::tests::server::documents::indexing_auto
{
	class RavenDB_8761 : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RavenDB_8761, can_group_by_array_values)
	{
		auto store = get_document_store(TEST_NAME);
		::RavenDB_8761::put_docs(store);
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders group by lines[].product \n"
				"  order by count() \n"
				"  select key() as productName, count() as count")
				->wait_for_non_stale_results()
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by({ "lines[].product" })
				->select_key({}, "productName")
				->select_count()
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for (auto products : { product_counts1, product_counts2 })
			{
				ASSERT_EQ(2, products.size());

				ASSERT_EQ("products/1", products[0]->productName);
				ASSERT_EQ(1, products[0]->count);

				ASSERT_EQ("products/2", products[1]->productName);
				ASSERT_EQ(2, products[1]->count);
			}
		}
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders\n"
				" group by lines[].product, shipTo.country\n"
				" order by count() \n"
				" select lines[].product as productName, shipTo.country as country, count() as count")
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by(std::vector<std::string>{ "lines[].product", "shipTo.country" })
				->select_key("lines[].product", "productName")
				->select_key("shipTo.country", "country")
				->select_count()
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for(auto products : {product_counts1, product_counts2})
			{
				ASSERT_EQ(2, products.size());

				ASSERT_EQ("products/1", products[0]->productName);
				ASSERT_EQ(1, products[0]->count);
				ASSERT_EQ("USA", products[0]->country);

				ASSERT_EQ("products/2", products[1]->productName);
				ASSERT_EQ(2, products[1]->count);
				ASSERT_EQ("USA", products[1]->country);
			}
		}
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders\n"
				" group by lines[].product, lines[].quantity\n"
				" order by lines[].quantity\n"
				" select lines[].product as productName, lines[].quantity as quantity, count() as count")
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by(std::vector<std::string>{"lines[].product", "lines[].quantity"})
				->select_key("lines[].product", "productName")
				->select_key("lines[].quantity", "quantity")
				->select_count()
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for (auto products : { product_counts1, product_counts2 })
			{
				ASSERT_EQ(3, products.size());

				ASSERT_EQ("products/1", products[0]->productName);
				ASSERT_EQ(1, products[0]->count);
				ASSERT_EQ(1, products[0]->quantity);

				ASSERT_EQ("products/2", products[1]->productName);
				ASSERT_EQ(1, products[1]->count);
				ASSERT_EQ(2, products[1]->quantity);

				ASSERT_EQ("products/2", products[2]->productName);
				ASSERT_EQ(1, products[2]->count);
				ASSERT_EQ(3, products[2]->quantity);
			}
		}
	}

	TEST_F(RavenDB_8761, can_group_by_array_content)
	{
		auto store = get_document_store(TEST_NAME);
		::RavenDB_8761::put_docs(store);
		{
			auto session = store->open_session();

			auto order_line1 = infrastructure::entities::OrderLine();
			order_line1.product = "products/1";
			order_line1.quantity = 1;

			auto order_line2 = infrastructure::entities::OrderLine();
			order_line2.product = "products/2";
			order_line2.quantity = 2;

			auto address = infrastructure::entities::Address();
			address.country = "USA";

			auto order = std::make_shared<infrastructure::entities::Order>();
			order->shipTo = address;
			order->lines.push_back(std::move(order_line1));
			order->lines.push_back(std::move(order_line2));

			session.store(order);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders group by array(lines[].product)\n"
				" order by count()\n"
				" select key() as products, count() as count")
				->wait_for_non_stale_results()
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by({ client::documents::queries::GroupBy::array("lines[].product") })
				->select_key({}, "products")
				->select_count()
				->order_by({ "count" })
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for (auto products : { product_counts1, product_counts2 })
			{
				ASSERT_EQ(2, products.size());

				ASSERT_EQ(1, products[0]->products.size());
				ASSERT_TRUE(std::find(products[0]->products.begin(), products[0]->products.end(), "products/2") !=
					products[0]->products.end());
				ASSERT_EQ(1, products[0]->count);

				ASSERT_EQ(2, products[1]->products.size());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/1") !=
					products[1]->products.end());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/2") !=
					products[1]->products.end());
				ASSERT_EQ(2, products[1]->count);
			}
		}
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders\n"
				" group by array(lines[].product), shipTo.country\n"
				" order by count()\n"
				" select lines[].product as products, shipTo.country as country, count() as count")
				->wait_for_non_stale_results()
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by({client::documents::queries::GroupBy::array("lines[].product"),
					client::documents::queries::GroupBy::field("shipTo.country")})
				->select_key("lines[].product", "products")
				->select_count()
				->order_by({ "count" })
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for (auto products : { product_counts1, product_counts2 })
			{
				ASSERT_EQ(2, products.size());

				ASSERT_EQ(1, products[0]->products.size());
				ASSERT_TRUE(std::find(products[0]->products.begin(), products[0]->products.end(), "products/2") !=
					products[0]->products.end());
				ASSERT_EQ(1, products[0]->count);

				ASSERT_EQ(2, products[1]->products.size());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/1") !=
					products[1]->products.end());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/2") !=
					products[1]->products.end());
				ASSERT_EQ(2, products[1]->count);
			}
		}
		{
			auto session = store->open_session();

			auto product_counts1 = session.advanced().raw_query<::RavenDB_8761::ProductCount>(
				"from Orders\n"
				" group by array(lines[].product), array(lines[].quantity)\n"
				" order by count()\n"
				" select lines[].product as products, lines[].quantity as quantities, count() as count")
				->wait_for_non_stale_results()
				->to_list();

			auto product_counts2 = session.advanced().document_query<infrastructure::entities::Order>()
				->group_by({ client::documents::queries::GroupBy::array("lines[].product"),
					client::documents::queries::GroupBy::array("lines[].quantity") })
				->select_key("lines[].product", "products")
				->select_key("lines[].quantity", "quantities")
				->select_count()
				->order_by({ "count" })
				->of_type<::RavenDB_8761::ProductCount>()
				->to_list();

			for (auto products : { product_counts1, product_counts2 })
			{
				ASSERT_EQ(2, products.size());

				ASSERT_EQ(1, products[0]->products.size());
				ASSERT_TRUE(std::find(products[0]->products.begin(), products[0]->products.end(), "products/2") !=
					products[0]->products.end());
				ASSERT_EQ(1, products[0]->count);
				ASSERT_EQ(1, products[0]->quantities.size());
				ASSERT_TRUE(std::find(products[0]->quantities.begin(), products[0]->quantities.end(), 3) !=
					products[0]->quantities.end());

				ASSERT_EQ(2, products[1]->products.size());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/1") !=
					products[1]->products.end());
				ASSERT_TRUE(std::find(products[1]->products.begin(), products[1]->products.end(), "products/2") !=
					products[1]->products.end());
				ASSERT_EQ(2, products[1]->count);
				ASSERT_EQ(2, products[1]->quantities.size());
				ASSERT_TRUE(std::find(products[1]->quantities.begin(), products[1]->quantities.end(), 1) !=
					products[1]->quantities.end());
				ASSERT_TRUE(std::find(products[1]->quantities.begin(), products[1]->quantities.end(), 2) !=
					products[1]->quantities.end());
			}
		}
	}
}