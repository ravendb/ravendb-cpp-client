#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "Company.h"
#include "EntityIdHelperUtil.h"
#include "MaintenanceOperationExecutor.h"
#include "ConcurrencyException.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_11058Test : public driver::RavenTestDriver
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

	TEST_F(RavenDB_11058Test, CanCopyAttachment)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";

			session.store(company, "companies/1");

			const char file1[] = { 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1, sizeof(file1));
			session.advanced().attachments()->store_attachment(company, "file1", file1_stream);

			const char file2[] = { 3,2,1 };
			std::stringstream file2_stream{};
			file2_stream.write(file2, sizeof(file2));
			session.advanced().attachments()->store_attachment(company, "file10", file2_stream);
			
			session.save_changes();
		}

		auto stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(2, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			auto new_company = std::make_shared<infrastructure::entities::Company>();
			new_company->name = "CF";

			session.store(new_company, "companies/2");

			auto old_company = session.load<infrastructure::entities::Company>("companies/1");
			session.advanced().attachments()->copy_attachment(old_company, "file1", new_company, "file2");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(3, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file1"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file2"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file10"));

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file1"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file2"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file10"));
		}
		{
			auto session = store->open_session();

			session.advanced().attachments()->copy_attachment("companies/1", "file1", "companies/2", "file3");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(4, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file1"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file2"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file3"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file10"));

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file1"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file2"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file3"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file10"));
		}
		{
			auto session = store->open_session();

			session.advanced().attachments()->copy_attachment("companies/1", "file1",
				"companies/2", "file3");//should throw
			ASSERT_THROW(session.save_changes();, exceptions::ConcurrencyException);
		}
	}

	TEST_F(RavenDB_11058Test, CanMoveAttachment)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";

			session.store(company, "companies/1");

			const char file1[] = { 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1, sizeof(file1));
			session.advanced().attachments()->store_attachment(company, "file1", file1_stream);

			const char file2[] = { 3,2,1 };
			std::stringstream file2_stream{};
			file2_stream.write(file2, sizeof(file2));
			session.advanced().attachments()->store_attachment(company, "file10", file2_stream);

			const char file3[] = { 4,5,6 };
			std::stringstream file3_stream{};
			file3_stream.write(file3, sizeof(file3));
			session.advanced().attachments()->store_attachment(company, "file20", file3_stream);

			session.save_changes();
		}

		auto stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(3, stats->count_of_attachments);
		ASSERT_EQ(3, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			auto new_company = std::make_shared<infrastructure::entities::Company>();
			new_company->name = "CF";
			session.store(new_company, "companies/2");

			auto old_company = session.load<infrastructure::entities::Company>("companies/1");

			session.advanced().attachments()->move_attachment(old_company, "file1", new_company, "file2");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(3, stats->count_of_attachments);
		ASSERT_EQ(3, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file1"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file2"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file10"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file20"));

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file1"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file2"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file10"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file20"));
		}
		{
			auto session = store->open_session();

			session.advanced().attachments()->move_attachment("companies/1", "file10", "companies/2", "file3");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(3, stats->count_of_attachments);
		ASSERT_EQ(3, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file1"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file2"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file3"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file10"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file20"));

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file1"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file2"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/2", "file3"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file10"));
			ASSERT_FALSE(session.advanced().attachments()->exists("companies/2", "file20"));
		}
		{
			auto session = store->open_session();

			session.advanced().attachments()->move_attachment("companies/1", "file20",
				"companies/2", "file3");//should throw

			ASSERT_THROW(session.save_changes(); , exceptions::ConcurrencyException);
		}
	}

	TEST_F(RavenDB_11058Test, CanRenameAttachment)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto company = std::make_shared<infrastructure::entities::Company>();
			company->name = "HR";

			session.store(company, "companies/1");

			const char file1[] = { 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1, sizeof(file1));
			session.advanced().attachments()->store_attachment(company, "file1", file1_stream);

			const char file2[] = { 3,2,1 };
			std::stringstream file2_stream{};
			file2_stream.write(file2, sizeof(file2));
			session.advanced().attachments()->store_attachment(company, "file10", file2_stream);

			session.save_changes();
		}

		auto stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(2, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");
			session.advanced().attachments()->rename_attachment(company, "file1", "file2");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(2, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file1"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file2"));
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");
			session.advanced().attachments()->rename_attachment(company, "file2", "file3");
			session.save_changes();
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(2, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);

		{
			auto session = store->open_session();

			ASSERT_FALSE(session.advanced().attachments()->exists("companies/1", "file2"));
			ASSERT_TRUE(session.advanced().attachments()->exists("companies/1", "file3"));
		}
		{
			auto session = store->open_session();

			auto company = session.load<infrastructure::entities::Company>("companies/1");
			session.advanced().attachments()->rename_attachment(company, "file3", "file10");//should throw
			ASSERT_THROW(session.save_changes(); , exceptions::ConcurrencyException);
		}

		stats = store->maintenance()->send(documents::operations::GetStatisticsOperation());
		ASSERT_EQ(2, stats->count_of_attachments);
		ASSERT_EQ(2, stats->count_of_unique_attachments);
	}
}
