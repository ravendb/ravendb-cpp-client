#include "pch.h"
#include "RequestExecutor.h"
#include "GetDatabaseRecordCommand.h"
#include "GetDatabaseNamesCommand.h"
#include "CreateDatabaseCommand.h"
#include "DeleteDatabaseCommand.h"


const std::string RAVEN_SERVER_URL = "http://localhost:8080";

static const std::string test_prefix{ "TEST__" };

class RequestExecutorTests : public ::testing::Test
{
	static bool contains_test_prefix(const std::string& str)
	{
		return (str.compare(0, test_prefix.length(), test_prefix) == 0);
	};
public:
	static const std::string cgdr_db;
	static const std::string cdd_db;
protected:

	static void SetUpTestCase() //make TEST DB(s)
	{
		
		auto&& executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
		ravendb::client::DatabaseRecord rec;
		rec.database_name = cdd_db;
		ravendb::client::CreateDatabaseCommand cmd1(rec, 1);
		executor->execute<ravendb::client::DatabasePutResult>(cmd1);

		rec.database_name = cgdr_db;
		ravendb::client::CreateDatabaseCommand cmd2(rec, 1);
		executor->execute<ravendb::client::DatabasePutResult>(cmd2);
	}

	static void TearDownTestCase() //delete all TEST DBs
	{
		auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
		ravendb::client::GetDatabaseNamesCommand cmd(0, 100);
		auto&& res = executor->execute<std::vector<std::string>>(cmd);
		for(const auto& dbname : res)
		{
			if (not contains_test_prefix(dbname))
			{
				continue;
			}
			ravendb::client::DeleteDatabaseCommand cmd2(dbname, true, {}, std::chrono::seconds(10));
			executor->execute<ravendb::client::DeleteDatabaseResult>(cmd2);
		}
	}
};

const std::string RequestExecutorTests::cgdr_db{ "TEST__CanGetDatabaseRecord" };
const std::string RequestExecutorTests::cdd_db{ "TEST__CanDeleteDatabase" };

TEST_F(RequestExecutorTests, CanGetDatabaseRecord)
{
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, cgdr_db);

	ravendb::client::GetDatabaseRecordCommand cmd(cgdr_db);

	auto&& rec = executor->execute<ravendb::client::DatabaseRecordWithEtag>(cmd);
	ASSERT_EQ(rec.database_name, cgdr_db);
}

TEST_F(RequestExecutorTests, CanGetDatabaseName)
{
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL },{});

	ravendb::client::GetDatabaseNamesCommand cmd(0, 100);

	auto&& res = executor->execute<std::vector<std::string>>(cmd);
	ASSERT_NE(std::find(res.begin(), res.end(), cgdr_db), res.end());
}


TEST_F(RequestExecutorTests, ThrowsWhenDatabaseDoesNotExist)
{
	try
	{
		auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, "no_such_db");
		//ravendb::client::GetNextOperationIdCommand cmd;
		//executor->execute(cmd);
	}
	catch (std::exception& exception)
	{
		auto ex = dynamic_cast<ravendb::client::RavenError*>(&exception);
		ASSERT_NE(ex, static_cast<decltype(ex)>(nullptr));
		ASSERT_EQ(ex->get_error_type(), ravendb::client::RavenError::ErrorType::database_does_not_exist);
		SUCCEED();
		return;
	}
	FAIL();
}

TEST_F(RequestExecutorTests, CanDeleteDatabase)
{
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
	{
		ravendb::client::DeleteDatabaseCommand cmd(cdd_db, true, {}, std::chrono::seconds(5));

		auto&& res = executor->execute<ravendb::client::DeleteDatabaseResult>(cmd);
		ASSERT_GT(res.raft_command_index, 0);
	}
	{
		ravendb::client::GetDatabaseNamesCommand cmd(0, 100);

		auto&& res = executor->execute<std::vector<std::string>>(cmd);
		ASSERT_EQ(std::find(res.begin(), res.end(), cdd_db), res.end());
	}
}


TEST_F (RequestExecutorTests, CanCreateDatabase)
{
	const std::string db{ "TEST__CanCreateDatabase" };

	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
	ravendb::client::DatabaseRecord rec;
	rec.database_name = db;
	ravendb::client::CreateDatabaseCommand cmd(rec, 2);

	auto&& res = executor->execute<ravendb::client::DatabasePutResult>(cmd);
	ASSERT_EQ(res.name, rec.database_name);
}












int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	RUN_ALL_TESTS();
}

