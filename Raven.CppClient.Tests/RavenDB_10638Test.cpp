#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_10638Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RavenDB_10638Test, AfterQueryExecutedShouldBeExecutedOnlyOnce)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();

		std::atomic_int32_t counter{ 0 };

		auto results = session.query<infrastructure::entities::User>()
			->add_after_query_executed_listener(std::function<void(const documents::queries::QueryResult&)>(
				[&] (const documents::queries::QueryResult&)->void { counter.fetch_add(1); }	))
			->where_equals("name", "Doe")
			->to_list();

		ASSERT_TRUE(results.empty());
		ASSERT_EQ(1, counter.load());
	}
}
