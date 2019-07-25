#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "MaintenanceOperationExecutor.h"
#include "GetDatabaseRecordOperation.h"
#include "UpdateDatabaseOperation.h"
#include "CreateDatabaseOperation.h"
#include "ConcurrencyException.h"
#include "User.h"
#include "DatabaseDisabledException.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_10929Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RavenDB_10929Test, CanUpdateDatabaseRecord)
	{
		auto store = get_document_store(TEST_NAME);

		auto record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(store->get_database()));

		auto etag = *record->etag;

		ASSERT_TRUE(record);
		ASSERT_GT(etag, 0);
		ASSERT_FALSE(record->disabled);

		record->disabled = true;

		store->maintenance()->server()->send(serverwide::operations::UpdateDatabaseOperation(*record, etag));

		record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(store->get_database()));
		ASSERT_TRUE(record);
		ASSERT_GT(*record->etag, etag);
		ASSERT_TRUE(record->disabled);

		const auto finally = [&]()
		{
			record->disabled = false;
			store->maintenance()->server()->send(serverwide::operations::UpdateDatabaseOperation(*record, *record->etag));
		};
		
		struct Finally
		{
			const std::function<void()> func;
			~Finally() { func(); }
				
		}finally_exec{finally};
		
		const auto record_copy = *record;

		ASSERT_THROW(store->maintenance()->server()->send(serverwide::operations::CreateDatabaseOperation(record_copy));
			,exceptions::ConcurrencyException);

		try
		{
			auto session = store->open_session();
			session.store(std::make_shared<infrastructure::entities::User>(), "users/1");
			session.save_changes();
		}
		catch (const std::exception& e)
		{
			ASSERT_THROW(std::rethrow_if_nested(e);,exceptions::database::DatabaseDisabledException);
			SUCCEED();
			return;
		}
		FAIL();
	}
}
