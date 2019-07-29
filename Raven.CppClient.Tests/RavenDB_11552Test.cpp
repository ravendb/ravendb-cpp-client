#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "Company.h"
#include "EntityIdHelperUtil.h"
#include "DeleteCommandData.h"
#include "PatchCommandData.h"
#include "PutAttachmentCommandData.h"
#include "DeleteAttachmentCommandData.h"
#include "CopyAttachmentCommandData.h"
#include "MoveAttachmentCommandData.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_11552Test : public driver::RavenTestDriver
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

	TEST_F(RavenDB_11552Test, PatchWillUpdateTrackedDocumentAfterSaveChanges)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";
			session.store(company, "companies/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");
			session.advanced().patch(company, "name", "CF");

			auto cv = session.advanced().get_change_vector_for(company);
			auto last_modified = session.advanced().get_last_modified_for(company);

			session.save_changes();

			ASSERT_EQ("CF", company->name);
			ASSERT_NE(cv, session.advanced().get_change_vector_for(company));
			ASSERT_NE(last_modified, session.advanced().get_last_modified_for(company));

			company->phone = 123;
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");

			ASSERT_EQ("CF", company->name);
			ASSERT_EQ(123, company->phone);
		}
	}

	TEST_F(RavenDB_11552Test, DeleteWillWork)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";
			session.store(company, "companies/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");

			ASSERT_TRUE(company);
			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			session.advanced().defer({ std::make_shared<documents::commands::batches::DeleteCommandData>(
				"companies/1", std::string{}) });
			session.save_changes();

			ASSERT_FALSE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());

			company = session.load<infrastructure::entities::Company>("companies/1");
			ASSERT_FALSE(company);
			ASSERT_FALSE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(3, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(RavenDB_11552Test, PatchWillWork)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";
			session.store(company, "companies/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");

			ASSERT_TRUE(company);
			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(1, session.advanced().get_number_of_requests());

			auto patch_request = documents::operations::PatchRequest();
			patch_request.script = "this.name = 'HR2';";

			session.advanced().defer({ std::make_shared<documents::commands::batches::PatchCommandData>(
				"companies/1", std::optional<std::string>{}, patch_request, std::optional<documents::operations::PatchRequest>{}) });

			session.save_changes();

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());

			auto company2 = session.load<infrastructure::entities::Company>("companies/1");

			ASSERT_TRUE(company2);
			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());
			ASSERT_EQ(*company, *company2);
			ASSERT_EQ("HR2", company2->name);
		}
	}
	TEST_F(RavenDB_11552Test, AttachmentPutAndDeleteWillWork)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";

			const char file0[] = { 1,2,3 };
			std::stringstream file0_stream{};
			file0_stream.write(file0, sizeof(file0));

			session.store(company, "companies/1");
			session.advanced().attachments()->store_attachment(company, "file0", file0_stream);
			session.save_changes();

			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company).size());
		}
		{
			auto session = store->open_session();
			auto company = session.load<infrastructure::entities::Company>("companies/1");

			ASSERT_TRUE(company);
			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(1, session.advanced().get_number_of_requests());
			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company).size());
			
			const char file1[] = { 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1, sizeof(file1));

			session.advanced().defer({ std::make_shared<documents::commands::batches::PutAttachmentCommandData>(
				"companies/1", "file1", file1_stream, std::optional<std::string>{}) });
			session.save_changes();

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());
			ASSERT_EQ(2, session.advanced().attachments()->get_attachments_names(company).size());

			session.advanced().defer({ std::make_shared<documents::commands::batches::DeleteAttachmentCommandData>(
				"companies/1", "file1") });
			session.save_changes();

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(3, session.advanced().get_number_of_requests());
			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company).size());
		}
	}

	TEST_F(RavenDB_11552Test, AttachmentCopyAndMoveWillWork)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company1 = std::make_shared<infrastructure::entities::Company>();
			company1->name = "HR";

			auto company2 = std::make_shared<infrastructure::entities::Company>();
			company2->name = "HR";

			session.store(company1, "companies/1");
			session.store(company2, "companies/2");

			const char file1[] = { 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1, sizeof(file1));

			session.advanced().attachments()->store_attachment(company1, "file1", file1_stream);
			session.save_changes();

			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company1).size());
		}
		{
			auto session = store->open_session();

			auto company1 = session.load<infrastructure::entities::Company>("companies/1");
			auto company2 = session.load<infrastructure::entities::Company>("companies/2");

			ASSERT_TRUE(company1);

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());
			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company1).size());

			ASSERT_TRUE(company2);

			ASSERT_TRUE(session.advanced().is_loaded("companies/2"));
			ASSERT_EQ(2, session.advanced().get_number_of_requests());
			ASSERT_TRUE(session.advanced().attachments()->get_attachments_names(company2).empty());

			session.advanced().defer({ std::make_shared<documents::commands::batches::CopyAttachmentCommandData>(
				"companies/1", "file1", "companies/2", "file1") });
			session.save_changes();

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(3, session.advanced().get_number_of_requests());
			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company1).size());

			ASSERT_TRUE(session.advanced().is_loaded("companies/2"));
			ASSERT_EQ(3, session.advanced().get_number_of_requests());
			ASSERT_EQ(1, session.advanced().attachments()->get_attachments_names(company2).size());

			session.advanced().defer({ std::make_shared<documents::commands::batches::MoveAttachmentCommandData>(
				"companies/1", "file1", "companies/2", "file2") });
			session.save_changes();

			ASSERT_TRUE(session.advanced().is_loaded("companies/1"));
			ASSERT_EQ(4, session.advanced().get_number_of_requests());
			ASSERT_TRUE(session.advanced().attachments()->get_attachments_names(company1).empty());

			ASSERT_TRUE(session.advanced().is_loaded("companies/2"));
			ASSERT_EQ(4, session.advanced().get_number_of_requests());
			ASSERT_EQ(2, session.advanced().attachments()->get_attachments_names(company2).size());
		}
	}
}
