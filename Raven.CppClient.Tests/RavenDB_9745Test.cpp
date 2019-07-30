#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "Company.h"

namespace RavenDB_9745Test_ns
{
	struct Result
	{
		std::string key{};
		int64_t count{};
	};

	void from_json(const nlohmann::json& j, Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "key", r.key);
		get_val_from_json(j, "count", r.count);
	}

	void to_json(nlohmann::json& j, const Result& r)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "key", r.key);
		set_val_to_json(j, "count", r.count);
	}

	class Companies_ByName : public ravendb::client::documents::indexes::AbstractIndexCreationTask
	{
	public:
		~Companies_ByName() override = default;
		Companies_ByName()
		{
			SET_DEFAULT_INDEX_NAME();

			map = "from c in docs.Companies select new { key = c.name, count = 1 }";

			reduce = "from result in results "
				"group result by result.key "
				"into g "
				"select new "
				"{ "
				"  key = g.Key, "
				"  count = g.Sum(x => x.count) "
				"}";

			store("key", ravendb::client::documents::indexes::FieldStorage::YES);
		}
	};
}

namespace ravendb::client::tests::issues
{
	class RavenDB_9745Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::Company, id);
		}
	};

	TEST_F(RavenDB_9745Test, Explain)
	{
		auto store = get_document_store(TEST_NAME);

		RavenDB_9745Test_ns::Companies_ByName().execute(store);

		{
			auto session = store->open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->name = "Micro";

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->name = "Microsoft";

			auto company3 = std::make_shared<infrastructure::entities::Company>();
			company3->name = "Google";

			session.store(company1);
			session.store(company2);
			session.store(company3);
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();

			auto explanations_reference = std::shared_ptr<documents::queries::explanation::Explanations>{};

			auto companies = session.advanced().document_query<infrastructure::entities::Company>()
				->include_explanation(explanations_reference)
				->search("name", "Micro*")
				->to_list();

			ASSERT_EQ(2, companies.size());

			auto exp = explanations_reference->get_explanations(companies.at(1)->id);
			ASSERT_TRUE(exp);
		}
		{
			auto session = store->open_session();

			auto options = std::optional<documents::queries::explanation::ExplanationOptions>(std::in_place);
			options->group_key = "key";

			auto explanations_reference = std::shared_ptr<documents::queries::explanation::Explanations>{};

			auto results = session.advanced()
				.document_query<RavenDB_9745Test_ns::Result, RavenDB_9745Test_ns::Companies_ByName>()
				->include_explanations(options, explanations_reference)
				->to_list();

			ASSERT_EQ(3, results.size());

			auto exp = explanations_reference->get_explanations(results.at(0)->key);
			ASSERT_TRUE(exp);

			exp = explanations_reference->get_explanations(results.at(1)->key);
			ASSERT_TRUE(exp);

			exp = explanations_reference->get_explanations(results.at(2)->key);
			ASSERT_TRUE(exp);
		}
	}
}