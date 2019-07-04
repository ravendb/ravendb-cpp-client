#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::tests::client::issues
{
	namespace ravendb_903_test
	{
		struct Product
		{
			std::string name{};
			std::string description{};
		};

		void to_json(nlohmann::json& j, const Product& p)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "name", p.name);
			set_val_to_json(j, "description", p.description);
		}

		void from_json(const nlohmann::json& j, Product& p)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "name", p.name);
			get_val_from_json(j, "description", p.description);
		}
	}

	class RavenDB903Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			store->set_before_perform(infrastructure::set_for_fiddler);
		}

		class TestIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~TestIndex() override = default;
			TestIndex()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from product in docs.Products select new { product.name, product.description }";

				index("description", documents::indexes::FieldIndexing::SEARCH);
			}
		};

		void do_test(std::function<std::shared_ptr<
			documents::session::IDocumentQuery<ravendb_903_test::Product, documents::session::DocumentQuery<ravendb_903_test::Product>>>(
				documents::session::DocumentSession&)> query_function)
		{
			auto store = get_document_store(TEST_NAME);
			store->execute_index(std::make_shared<TestIndex>());

			{
				auto session = store->open_session();

				auto product1 = std::make_shared<ravendb_903_test::Product>();
				product1->name = "Foo";
				product1->description = "Hello World";

				auto product2 = std::make_shared<ravendb_903_test::Product>();
				product2->name = "Bar";
				product2->description = "Hello World";

				auto product3 = std::make_shared<ravendb_903_test::Product>();
				product3->name = "Bar";
				product3->description = "Goodbye World";

				session.store(product1);
				session.store(product2);
				session.store(product3);
				session.save_changes();
			}
			wait_for_indexing(store);

			{
				auto session = store->open_session();
				auto query = query_function(session);

				auto products = query->to_list();
				ASSERT_EQ(1, products.size());
			}
		}
	};

	TEST_F(RavenDB903Test, Test1)
	{
		do_test([](documents::session::DocumentSession& session)
		{
			return session.advanced().document_query<ravendb_903_test::Product, TestIndex>()
				->search("description", "Hello")
				->intersect()
				->where_equals("name", "Bar");
		});
	}

	TEST_F(RavenDB903Test, Test)
	{
		do_test([](documents::session::DocumentSession& session)
		{
			return session.advanced().document_query<ravendb_903_test::Product, TestIndex>()
				->where_equals("name", "Bar")
				->intersect()
				->search("description", "Hello");
		});
	}
}
