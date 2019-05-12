#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "EntityIdHelperUtil.h"
#include "LazySessionOperations.h"

namespace load_all_starting_with
{
	struct Abc
	{
		std::string id{};
	};

	void from_json(const nlohmann::json& j, Abc& abc)
	{}

	void to_json(nlohmann::json& j, const Abc& abc)
	{}

	struct Xyz
	{
		std::string id{};
	};

	void from_json(const nlohmann::json& j, Xyz& xyz)
	{}

	void to_json(nlohmann::json& j, const Xyz& xyz)
	{}


}

namespace ravendb::client::tests::mailing_list
{
	class LoadAllStartingWith : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(load_all_starting_with::Abc, id);
			REGISTER_ID_PROPERTY_FOR(load_all_starting_with::Xyz, id);
		}
	};

	TEST_F(LoadAllStartingWith, LoadAllStartingWith)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto abc1 = std::make_shared<load_all_starting_with::Abc>();
			abc1->id = "abc/1";
			auto abc2 = std::make_shared<load_all_starting_with::Abc>();
			abc2->id = "abc/2";

			auto xyz1 = std::make_shared<load_all_starting_with::Xyz>();
			xyz1->id = "xyz/1";

			session.store(abc1);
			session.store(abc2);
			session.store(xyz1);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto load_results1 = session.advanced().load_starting_with<load_all_starting_with::Abc>("abc/");
			auto load_results2 = session.advanced().load_starting_with<load_all_starting_with::Xyz>("xyz/");

			ASSERT_EQ(2, load_results1.size());
			ASSERT_EQ("abc/1", load_results1[0]->id);

			ASSERT_EQ(1, load_results2.size());
			ASSERT_EQ("xyz/1", load_results2[0]->id);
		}

		{
			auto session = store->open_session();

			auto load_results1 = session
				.advanced()
				.lazily()
				.load_starting_with<load_all_starting_with::Abc>("abc/")
				.get_value();
			auto load_results2 = session
				.advanced()
				.lazily()
				.load_starting_with<load_all_starting_with::Xyz>("xyz/")
				.get_value();

			ASSERT_EQ(2, load_results1.size());
			ASSERT_EQ("abc/1", load_results1["abc/1"]->id);

			ASSERT_EQ(1, load_results2.size());
			ASSERT_EQ("xyz/1", load_results2["xyz/1"]->id);
		}
	}
}

