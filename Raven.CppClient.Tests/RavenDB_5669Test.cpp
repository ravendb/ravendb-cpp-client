#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::tests::client::issues
{
	namespace ravendb_5669_test
	{
		struct Animal
		{
			std::string type{};
			std::string name{};
			
		};

		void to_json(nlohmann::json& j, const Animal& a)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "name", a.name);
			set_val_to_json(j, "type", a.type);
		}

		void from_json(const nlohmann::json& j, Animal& a)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "name", a.name);
			get_val_from_json(j, "type", a.type);
		}
	}

	class RavenDB_5669Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		class Animal_Index : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Animal_Index() override = default;
			Animal_Index()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from animal in docs.Animals select new { name = animal.name, type = animal.type }";

				analyze("name", "StandardAnalyzer");
				index("name", documents::indexes::FieldIndexing::SEARCH);
			}
		};

		void store_animals(std::shared_ptr<documents::DocumentStore> store)
		{
			{
				auto session = store->open_session();

				auto animal1 = std::make_shared<ravendb_5669_test::Animal>();
				animal1->name = "Peter Pan";
				animal1->type = "Dog";

				auto animal2 = std::make_shared<ravendb_5669_test::Animal>();
				animal2->name = "Peter Poo";
				animal2->type = "Dog";

				auto animal3 = std::make_shared<ravendb_5669_test::Animal>();
				animal3->name = "Peter Foo";
				animal3->type = "Dog";

				session.store(animal1);
				session.store(animal2);
				session.store(animal3);
				session.save_changes();
			}

			wait_for_indexing(store, store->get_database());
		}
	};

	TEST_F(RavenDB_5669Test, WorkingTestWithDifferentSearchTermOrder)
	{
		auto store = get_document_store(TEST_NAME);

		store->execute_index(std::make_shared<Animal_Index>());

		store_animals(store);

		{
			auto session = store->open_session();

			auto query = session.advanced().document_query<ravendb_5669_test::Animal, Animal_Index>();

			query->open_subclause();

			query = query->where_equals("type", "Cat");
			query = query->or_else();
			query = query->search("name", "Peter*");
			query = query->and_also();
			query = query->search("name", "Pan*");

			query->close_subclause();

			auto results = query->to_list();
			ASSERT_EQ(1, results.size());
		}
	}


	TEST_F(RavenDB_5669Test, WorkingTestWithSubclause)
	{
		auto store = get_document_store(TEST_NAME);

		store->execute_index(std::make_shared<Animal_Index>());

		store_animals(store);

		{
			auto session = store->open_session();

			auto query = session.advanced().document_query<ravendb_5669_test::Animal, Animal_Index>();

			query->open_subclause();

			query = query->where_equals("type", "Cat");
			query = query->or_else();

			query->open_subclause();

			query = query->search("name", "Pan*");
			query = query->and_also();
			query = query->search("name", "Peter*");
			query = query->close_subclause();

			query->close_subclause();

			auto results = query->to_list();
			ASSERT_EQ(1, results.size());
		}
	}
}
