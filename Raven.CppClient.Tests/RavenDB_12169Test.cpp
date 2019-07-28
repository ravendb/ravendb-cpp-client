#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "Company.h"
#include "BatchPatchCommandData.h"
#include "ConcurrencyException.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_12169Test : public driver::RavenTestDriver
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

	TEST_F(RavenDB_12169Test, CanUseBatchPatchCommand)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->id = "companies/1";
			company1->name = "C1";
			session.store(company1);

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->id = "companies/2";
			company2->name = "C2";
			session.store(company2);

			auto company3 = std::make_shared<infrastructure::entities::Company>();
			company3->id = "companies/3";
			company3->name = "C3";
			session.store(company3);

			auto company4 = std::make_shared<infrastructure::entities::Company>();
			company4->id = "companies/4";
			company4->name = "C4";
			session.store(company4);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto c1 = session.load<infrastructure::entities::Company>("companies/1");
			auto c2 = session.load<infrastructure::entities::Company>("companies/2");
			auto c3 = session.load<infrastructure::entities::Company>("companies/3");
			auto c4 = session.load<infrastructure::entities::Company>("companies/4");

			ASSERT_EQ("C1", c1->name);
			ASSERT_EQ("C2", c2->name);
			ASSERT_EQ("C3", c3->name);
			ASSERT_EQ("C4", c4->name);

			const std::vector<std::string> ids{ c1->id, c3->id };

			session.advanced().defer({ std::make_shared<documents::commands::batches::BatchPatchCommandData>(
				documents::operations::PatchRequest("this.name = 'test'; "),
				std::optional<documents::operations::PatchRequest>(),
				ids) });

			session.advanced().defer({ std::make_shared<documents::commands::batches::BatchPatchCommandData>(
				documents::operations::PatchRequest("this.name = 'test2'; "),
				std::optional<documents::operations::PatchRequest>(),
				std::vector<std::string>{c4->id}) });

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto c1 = session.load<infrastructure::entities::Company>("companies/1");
			auto c2 = session.load<infrastructure::entities::Company>("companies/2");
			auto c3 = session.load<infrastructure::entities::Company>("companies/3");
			auto c4 = session.load<infrastructure::entities::Company>("companies/4");

			ASSERT_EQ("test", c1->name);
			ASSERT_EQ("C2", c2->name);
			ASSERT_EQ("test", c3->name);
			ASSERT_EQ("test2", c4->name);
		}
		{
			auto session = store->open_session();

			auto c2 = session.load<infrastructure::entities::Company>("companies/2");
			session.advanced().defer({ std::make_shared<documents::commands::batches::BatchPatchCommandData>(
				documents::operations::PatchRequest("this.name = 'test2'; "),
				std::optional<documents::operations::PatchRequest>(),
				std::vector<documents::commands::batches::BatchPatchCommandData::IdAndChangeVector>{
					documents::commands::batches::BatchPatchCommandData::IdAndChangeVector::create(
						c2->id, "invalidCV")})});

			ASSERT_THROW(session.save_changes();,
				exceptions::ConcurrencyException);
		}
		{
			auto session = store->open_session();

			auto c1 = session.load<infrastructure::entities::Company>("companies/1");
			auto c2 = session.load<infrastructure::entities::Company>("companies/2");
			auto c3 = session.load<infrastructure::entities::Company>("companies/3");
			auto c4 = session.load<infrastructure::entities::Company>("companies/4");

			ASSERT_EQ("test", c1->name);
			ASSERT_EQ("C2", c2->name);
			ASSERT_EQ("test", c3->name);
			ASSERT_EQ("test2", c4->name);
		}
	}
}
