#include "pch.h"
#include "RequestExecutor.h"
#include "GetDatabaseRecordCommand.h"
#include "GetDatabaseNamesCommand.h"
#include "CreateDatabaseCommand.h"
#include "DeleteDatabaseCommand.h"

using namespace ravendb::client;

class RequestExecutorTests : public ::testing::Test
{
public:
	static inline auto nodb_executor = http::RequestExecutor::create({ RAVEN_SERVER_URL }, {},{}, set_for_fiddler);

protected:

	static inline const std::string test_can_get_database_record = "test_database_1";
	static inline const std::string test_can_delete_database = "test_database_2";

	static void SetUpTestCase() //make TEST DB(s)
	{
		serverwide::DatabaseRecord rec;
		rec.database_name = test_can_get_database_record;
		serverwide::operations::CreateDatabaseCommand cmd1(rec, 1);
		nodb_executor->execute<serverwide::operations::DatabasePutResult>(cmd1);

		rec.database_name = test_can_delete_database;
		serverwide::operations::CreateDatabaseCommand cmd2(rec, 1);
		nodb_executor->execute<serverwide::operations::DatabasePutResult>(cmd2);
	}

	static void TearDownTestCase() //delete TEST DBs
	{
		serverwide::operations::DeleteDatabaseCommand cmd1(test_can_get_database_record, true, {}, std::chrono::seconds(10));
		nodb_executor->execute<serverwide::operations::DeleteDatabaseResult>(cmd1);
		try
		{
			serverwide::operations::DeleteDatabaseCommand cmd1(test_can_delete_database, true, {}, std::chrono::seconds(10));
			nodb_executor->execute<serverwide::operations::DeleteDatabaseResult>(cmd1);
		}catch (...)
		{}
	}
};

TEST_F(RequestExecutorTests, CanGetDatabaseRecord)
{
	serverwide::operations::GetDatabaseRecordCommand cmd(test_can_get_database_record);

	auto rec = nodb_executor->execute(cmd);
	ASSERT_EQ(rec.database_name, test_can_get_database_record);
	ASSERT_GT(rec.etag.value(), -1);
}

TEST_F(RequestExecutorTests, CanGetDatabaseName)
{
	serverwide::operations::GetDatabaseNamesCommand cmd(0, 100);

	auto&& res = nodb_executor->execute<std::vector<std::string>>(cmd);
	ASSERT_NE(std::find(res.begin(), res.end(), test_can_get_database_record), res.end());
}

TEST_F(RequestExecutorTests, ThrowsWhenDatabaseDoesNotExist)
{//TODO refactor this for using exceptions and async (when reqexec.create will call update_topology async)
	try
	{
		auto executor = http::RequestExecutor::create({ RAVEN_SERVER_URL }, "no_such_db");
		//when async, execute will make await
		//ravendb::client::GetNextOperationIdCommand cmd;
		//executor->execute(cmd);
	}
	catch (RavenError& exception)
	{
		ASSERT_EQ(exception.get_error_type(), RavenError::ErrorType::database_does_not_exist);
		SUCCEED();
		return;
	}
	FAIL();
}

TEST_F(RequestExecutorTests, CanDeleteDatabase)
{
	{
		serverwide::operations::DeleteDatabaseCommand cmd(test_can_delete_database, true, {}, std::chrono::seconds(5));
		auto&& res = nodb_executor->execute<serverwide::operations::DeleteDatabaseResult>(cmd);
		ASSERT_GT(res.raft_command_index, 0);
	}
	{
		serverwide::operations::GetDatabaseNamesCommand cmd(0, 100);
		auto&& res = nodb_executor->execute<std::vector<std::string>>(cmd);
		ASSERT_EQ(std::find(res.begin(), res.end(), test_can_delete_database), res.end());
	}
}
//
//TEST_F (RequestExecutorTests, CanCreateDatabase)
//{
//	const std::string db{ "TEST__CanCreateDatabase" };
//
//	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
//	ravendb::client::serverwide::DatabaseRecord rec;
//	rec.database_name = db;
//	ravendb::client::serverwide::operations::CreateDatabaseCommand cmd(rec, 2);
//
//	auto&& res = executor->execute<ravendb::client::serverwide::operations::DatabasePutResult>(cmd);
//	ASSERT_EQ(res.name, rec.database_name);
//}
