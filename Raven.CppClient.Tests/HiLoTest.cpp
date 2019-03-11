#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "HiLoIdGenerator.h"
#include "MultiDatabaseHiLoIdGenerator.h"
#include "User.h"
#include "re_definitions.h"

namespace hilo_test
{
	struct HiloDoc
	{
		int64_t max{};
	};

	inline void to_json(nlohmann::json& j, const HiloDoc& hld)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Max", hld.max);
	}

	inline void from_json(const nlohmann::json& j, HiloDoc& hld)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Max", hld.max);
	}

	struct Product
	{
		std::string product_name{};
	};

	inline void to_json(nlohmann::json& j, const Product& pn)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "ProductName", pn.product_name);
	}

	inline void from_json(const nlohmann::json& j, Product& pn)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "ProductName", pn.product_name);
	}
}


namespace ravendb::client::tests::client
{
	class HiloTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(HiloTest, HiloCanNotGoDown)
	{
		auto store = test_suite_store->get();
		{
			auto session = store->open_session();

			auto hilo_doc = std::make_shared<hilo_test::HiloDoc>();
			hilo_doc->max = 32;

			session.store(hilo_doc, "Raven/Hilo/users");
			session.save_changes();

			auto hilo_key_generator = documents::identity::HiLoIdGenerator("users",
				store, store->get_database(), store->get_conventions()->get_identity_part_separator());

			auto ids = std::vector<int64_t>();
			ids.push_back(hilo_key_generator.next_id());

			hilo_doc->max = 12;
			session.store(hilo_doc, {}, "Raven/Hilo/users");
			session.save_changes();

			for(auto i=0; i< 128; ++i)
			{
				auto next_id = hilo_key_generator.next_id();
				ASSERT_FALSE(std::find(ids.cbegin(), ids.cend(), next_id) != ids.cend());
				ids.push_back(next_id);
			}

			ASSERT_EQ(ids.size(), std::unordered_set<int64_t>(ids.begin(), ids.end()).size());
		}
	}

	TEST_F(HiloTest, HiLoMultiDb)
	{
		auto store = test_suite_store->get();

		auto session = store->open_session();
		auto hilo_doc = std::make_shared<hilo_test::HiloDoc>();
		hilo_doc->max = 64;
		session.store(hilo_doc, "Raven/Hilo/users");

		auto products_hilo = std::make_shared<hilo_test::HiloDoc>();
		products_hilo->max = 128;
		session.store(products_hilo, "Raven/Hilo/products");

		session.save_changes();

		auto multi_db_hilo = documents::identity::MultiDatabaseHiLoIdGenerator(store, store->get_conventions());
		auto generate_document_key = multi_db_hilo.generate_document_id({}, typeid(infrastructure::entities::User),
			std::make_shared<infrastructure::entities::User>());
		ASSERT_EQ("users/65", generate_document_key->substr(0, 8));

		generate_document_key = multi_db_hilo.generate_document_id({}, typeid(hilo_test::Product),
			std::make_shared<hilo_test::Product>());
		ASSERT_EQ("products/129", generate_document_key->substr(0, 12));
	}

	TEST_F(HiloTest, CapacityShouldDouble)
	{
		auto store = test_suite_store->get();

		auto hilo_id_generator = documents::identity::HiLoIdGenerator("users", store, store->get_database(),
			store->get_conventions()->get_identity_part_separator());
		{
			auto session = store->open_session();
			auto hilo_doc = std::make_shared<hilo_test::HiloDoc>();
			hilo_doc->max = 64;
			session.store(hilo_doc, "Raven/Hilo/users");
			session.save_changes();

			for(auto i = 0; i < 32 ; ++i)
			{
				hilo_id_generator.generate_document_id();
			}
		}
		{
			auto session = store->open_session();
			auto hilo_doc = session.load<hilo_test::HiloDoc>("Raven/Hilo/users");
			ASSERT_EQ(96, hilo_doc->max);

			//we should be receiving a range of 64 now
			hilo_id_generator.generate_document_id();
		}
		{
			auto session = store->open_session();
			auto hilo_doc = session.load<hilo_test::HiloDoc>("Raven/Hilo/users");
			ASSERT_EQ(160, hilo_doc->max);
		}
	}

	TEST_F(HiloTest, ReturnUnusedRangeOnClose)
	{
		auto store = test_suite_store->get();

		auto new_store = documents::DocumentStore::create();
		new_store->set_urls(store->get_urls());
		new_store->set_database(store->get_database());
#ifdef __USE_FIDDLER__
		new_store->set_before_perform(definitions::set_for_fiddler);
#endif

		new_store->initialize();

		{
			auto session = new_store->open_session();
			auto hilo_doc = std::make_shared<hilo_test::HiloDoc>();
			hilo_doc->max = 32;
			session.store(hilo_doc, "Raven/Hilo/users");
			session.save_changes();

			session.store(std::make_shared<infrastructure::entities::User>());
			session.store(std::make_shared<infrastructure::entities::User>());
			session.save_changes();
		}

		new_store->close();//on document store close, hilo-return should be called

		new_store = documents::DocumentStore::create();
		new_store->set_urls(store->get_urls());
		new_store->set_database(store->get_database());
#ifdef __USE_FIDDLER__
		new_store->set_before_perform(definitions::set_for_fiddler);
#endif
		new_store->initialize();

		{
			auto session = new_store->open_session();
			auto hilo_doc = session.load<hilo_test::HiloDoc>("Raven/Hilo/users");
			ASSERT_EQ(34, hilo_doc->max);
		}

		new_store->close();
	}

	TEST_F(HiloTest, DoesNotGetAnotherRangeWhenDoingParallelRequests)
	{
		//TODO implement
	}
}