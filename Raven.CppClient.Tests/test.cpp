#include "pch.h"
#include "../Raven.CppClient/RequestExecutor.h"
#include "../Raven.CppClient/GetDatabaseRecordCommand.h"
#include "../Raven.CppClient/GetDatabaseNamesCommand.h"
#include "../Raven.CppClient/CreateDatabaseCommand.h"
#include "../Raven.CppClient/DeleteDatabaseCommand.h"


const std::string RAVEN_SERVER_URL = "http://localhost:8080";
const std::string RAVEN_SERVER_DB = "Northwind";

class RequestExecutorTests : public ::testing::Test
{
	inline static const std::string test_prefix{ "TEST__" };
	static bool contains_text_prefix(const std::string& str)
	{
		return (str.compare(0, test_prefix.length(), test_prefix) == 0);
	};

protected:
	static void TearDownTestCase()
	{
		auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
		ravendb::GetDatabaseNamesCommand cmd(0, 100);
		auto&& res = executor->execute<std::vector<std::string>>(cmd);
		for(const auto& dbname : res)
		{
			if (not contains_text_prefix(dbname))
			{
				continue;
			}
			ravendb::DeleteDatabaseCommand cmd(dbname, true, {}, std::chrono::seconds(5));
			executor->execute<ravendb::DeleteDatabaseResult>(cmd);
		}
	}
};


TEST_F(RequestExecutorTests, CanGetDatabaseRecord)
{
	auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL }, RAVEN_SERVER_DB);

	ravendb::GetDatabaseRecordCommand cmd(RAVEN_SERVER_DB);

	auto&& rec = executor->execute<ravendb::DatabaseRecordWithEtag>(cmd);
	ASSERT_EQ(rec.database_name, RAVEN_SERVER_DB);
}

TEST_F(RequestExecutorTests, CanGetDatabaseName)
{
	auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL },{});

	ravendb::GetDatabaseNamesCommand cmd(0, 100);

	auto&& res = executor->execute<std::vector<std::string>>(cmd);
	ASSERT_NE(std::find(res.begin(), res.end(), RAVEN_SERVER_DB), res.end());
}


TEST_F(RequestExecutorTests, ThrowsWhenDatabaseDoesNotExist)
{
	try
	{
		auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL }, "no_such_db");
		//ravendb::GetNextOperationIdCommand cmd;
		//executor->execute(cmd);
	}
	catch (std::exception& exception)
	{
		auto ex = dynamic_cast<ravendb::RavenError*>(&exception);
		ASSERT_NE(ex, static_cast<decltype(ex)>(nullptr));
		ASSERT_EQ(ex->get_error_type(), ravendb::RavenError::ErrorType::database_does_not_exist);
		SUCCEED();
		return;
	}
	FAIL();
}

TEST_F(RequestExecutorTests, CanDeleteDatabase)
{
	const std::string db{ "TEST__CanDeleteDatabase" };

	auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
	{
		ravendb::DeleteDatabaseCommand cmd(db, true, {}, std::chrono::seconds(5));

		auto&& res = executor->execute<ravendb::DeleteDatabaseResult>(cmd);
		ASSERT_GT(res.raft_command_index, 0);
	}
	{
		ravendb::GetDatabaseNamesCommand cmd(0, 100);

		auto&& res = executor->execute<std::vector<std::string>>(cmd);
		ASSERT_EQ(std::find(res.begin(), res.end(), db), res.end());
	}

}


TEST_F (RequestExecutorTests, CanCreateDatabase)
{
	const std::string db{ "TEST__CanCreateDatabase" };

	auto executor = ravendb::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
	ravendb::DatabaseRecord rec;
	rec.database_name = db;
	ravendb::CreateDatabaseCommand cmd(rec, 1);

	auto&& res = executor->execute<ravendb::DatabasePutResult>(cmd);
	ASSERT_EQ(res.name, rec.database_name);

}












int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	RUN_ALL_TESTS();
}

