#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "WaitForRaftIndexCommand.h"
#include "MaintenanceOperationExecutor.h"
#include "GetDatabaseRecordOperation.h"
#include "UpdateDatabaseOperation.h"
#include "CreateDatabaseOperation.h"
#include "ConcurrencyException.h"
#include "User.h"
#include "DatabaseDisabledException.h"
#include "finally.hpp"

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

		auto database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(store->get_database()));

		const auto etag = *database_record->etag;

		ASSERT_TRUE(database_record);
		ASSERT_GT(etag, 0);
		ASSERT_FALSE(database_record->disabled);

		database_record->disabled = true;

		const auto update_result = store->maintenance()->server()->send(serverwide::operations::UpdateDatabaseOperation(*database_record, etag));

		//make sure to wait for the update to complete
		auto wait_for_raft_index_command = serverwide::commands::WaitForRaftIndexCommand(update_result->raft_command_index);
		store->get_request_executor()->execute(wait_for_raft_index_command);

		database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(store->get_database()));
		
		ASSERT_TRUE(database_record);
		ASSERT_GT(*database_record->etag, etag);
		ASSERT_TRUE(database_record->disabled);

		const auto finally = [&]()
		{
			database_record->disabled = false;
			store->maintenance()->server()->send(serverwide::operations::UpdateDatabaseOperation(*database_record, *database_record->etag));
		};
		
		struct Finally
		{
			const std::function<void()> func;
			~Finally() { func(); }
				
		}finally_exec{finally};
		
		const auto record_copy = *database_record;

		ASSERT_THROW(store->maintenance()->server()->send(serverwide::operations::CreateDatabaseOperation(record_copy));
			,exceptions::ConcurrencyException);

		//make sure to wait for database to be actually disabled...
		auto start = std::chrono::high_resolution_clock::now();
		try
		{
			while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() <= 120) //two minutes should be enough!
			{
				const auto databaseStats = store->maintenance()->send(documents::operations::GetStatisticsOperation());			
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}

			FAIL() << "The database disable command was accepted by Raft but the database wasn't disabled in 120 sec. It shouldn't be happening and is likely an indicator of an issue.";
		}
		catch(const std::exception& e)
		{
			ASSERT_THROW(std::rethrow_if_nested(e),exceptions::database::DatabaseDisabledException);
		}


		auto session = store->open_session();
		session.store(std::make_shared<infrastructure::entities::User>(), "users/1");
		database_record = store->maintenance()->server()->send(serverwide::operations::GetDatabaseRecordOperation(store->get_database()));

		try
		{
			session.save_changes();
		}
		catch (const std::exception& e)
		{
			ASSERT_THROW(std::rethrow_if_nested(e),exceptions::database::DatabaseDisabledException);
			SUCCEED() << "Trying to commit a transaction in a disabled database should throw DatabaseDisabledException";
			return;
		}

		FAIL() << "Saved a file into a database disabled via it's record and no exception was thrown. (Expected a DatabaseDisabledException to be thrown)";
		
	} //here runs the "finally" lambda
}
