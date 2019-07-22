#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "Company.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_9587Test : public driver::RavenTestDriver
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

	TEST_F(RavenDB_9587Test, TimingsShouldWork)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->name = "CF";
			session.store(company1);

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->name = "HR";
			session.store(company2);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::shared_ptr<documents::queries::timings::QueryTimings> timing_reference{};

			auto companies = session.query<infrastructure::entities::Company>()
				->timings(timing_reference)
				->where_not_equals("name", "HR")
				->to_list();

			ASSERT_GT(timing_reference->duration_in_ms, 0);
			ASSERT_TRUE(timing_reference->timings.has_value());
		}
	}
}
