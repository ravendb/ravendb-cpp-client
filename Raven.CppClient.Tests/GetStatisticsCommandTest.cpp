#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "CreateSampleDataOperation.h"
#include "GetStatisticsOperation.h"
#include "MaintenanceOperationExecutor.h"

namespace ravendb::client::tests::client::documents::commands
{
	class GetStatisticsCommandTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(GetStatisticsCommandTest, CanGetStats)
	{
		auto store = get_document_store(TEST_NAME);

		auto sample_data_operation = infrastructure::CreateSampleDataOperation();
		store->get_maintenance()->send(sample_data_operation);

		wait_for_indexing(store, store->get_database());

		auto command = ravendb::client::documents::operations::GetStatisticsOperation().get_command(store->get_conventions());
		store->get_request_executor()->execute(*command);

		auto stats = command->get_result();

		ASSERT_TRUE(stats->last_doc_etag.has_value());
		ASSERT_GT(stats->last_doc_etag, 0);

		ASSERT_EQ(3, stats->count_of_indexes);
		ASSERT_EQ(1059, stats->count_of_documents);
		ASSERT_GT(stats->count_of_revision_documents, 0);
		ASSERT_EQ(0, stats->count_of_documents_conflicts);
		ASSERT_EQ(0, stats->count_of_conflicts);
		ASSERT_EQ(17, stats->count_of_unique_attachments);
		ASSERT_FALSE(stats->database_change_vector.empty());
		ASSERT_FALSE(stats->database_id.empty());
		ASSERT_FALSE(stats->pager.empty());
		ASSERT_TRUE(stats->last_indexing_time.has_value());
		ASSERT_FALSE(stats->indexes.empty());
		ASSERT_FALSE(stats->size_on_disk.humane_size.empty());
		ASSERT_GT(stats->size_on_disk.size_in_bytes, 0);

		for (const auto& index_info : stats->indexes)
		{
			ASSERT_FALSE(index_info.name.empty());
			ASSERT_FALSE(index_info.is_stale);
			ASSERT_NE(index_info.state, ravendb::client::documents::indexes::IndexState::UNSET);
			ASSERT_NE(index_info.lock_mode, ravendb::client::documents::indexes::IndexLockMode::UNSET);
			ASSERT_NE(index_info.priority, ravendb::client::documents::indexes::IndexPriority::UNSET);
			ASSERT_NE(index_info.type, ravendb::client::documents::indexes::IndexType::UNSET);
			ASSERT_NE(index_info.last_indexing_time, impl::DateTimeOffset());
		}
	}
}
