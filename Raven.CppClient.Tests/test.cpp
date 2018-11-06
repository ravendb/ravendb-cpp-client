#include "pch.h"
#include "../Raven.CppClient/RequestExecutor.h"
#include "../Raven.CppClient/GetDatabaseTopologyCommand.h"
#include "../Raven.CppClient/GetDatabaseRecordCommand.h"
#include "../Raven.CppClient/GetNextOperationIdCommand.h"


const std::string RAVEN_SERVER_URL = "http://localhost:8080";
const std::string RAVEN_SERVER_DB = "Northwind";

TEST(BasicRequestExecutorTests, CanGetDatabaseRecord) 
{
	auto rq = ravenDB::RequestExecutor::create({ RAVEN_SERVER_URL }, RAVEN_SERVER_DB);

	ravenDB::GetDatabaseRecordCommand cmd(RAVEN_SERVER_DB);

	auto rec = rq->execute<ravenDB::DatabaseRecordWithEtag>(cmd);

	ASSERT_EQ(RAVEN_SERVER_DB, rec.database_name);
}

TEST(BasicRequestExecutorTests, throwsWhenDatabaseDoesNotExist)
{
	try
	{
		auto rq = ravenDB::RequestExecutor::create({ RAVEN_SERVER_URL }, "no_such_db");
		ravenDB::GetNextOperationIdCommand cmd;
		rq->execute(cmd);
	}
	catch (std::exception& exception)
	{
		auto ex = dynamic_cast<ravenDB::RavenError*>(&exception);
		ASSERT_NE(ex, static_cast<decltype(ex)>(nullptr));
		ASSERT_EQ(ex->get_error_type(), ravenDB::RavenError::ErrorType::database_does_not_exist);
		SUCCEED();
		return;
	}
	FAIL();
}