#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::tests::client::queries
{
	namespace regex_query_test
	{
		struct RegexMe
		{
			std::string text{};

			RegexMe() = default;
			RegexMe(std::string text_param)
				: text(std::move(text_param))
			{}
		};

		void to_json(nlohmann::json& j, const RegexMe& rm)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "text", rm.text);
		}

		void from_json(const nlohmann::json& j, RegexMe& rm)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "text", rm.text);
		}
	}

	class RegexQueryTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RegexQueryTest, QueriesWithRegexFromDocumentQuery)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();
			const auto store_items = [&](const char* text)
			{
				session.store(std::make_shared<regex_query_test::RegexMe>(text));
			};
			store_items("I love dogs and cats");
			store_items("I love cats");
			store_items("I love dogs");
			store_items("I love bats");
			store_items("dogs love me");
			store_items("cats love me");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto query = session
				.advanced()
				.document_query<regex_query_test::RegexMe>()
				->where_regex("text", "^[a-z ]{2,4}love");

			auto index_query = query->get_index_query();

			ASSERT_EQ("from RegexMes where regex(text, $p0)", index_query.query);
			ASSERT_EQ("^[a-z ]{2,4}love", index_query.query_parameters.at("p0"));

			auto result = query->to_list();
			ASSERT_EQ(4, result.size());
		}
	}

}
