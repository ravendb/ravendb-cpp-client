#include "pch.h"
#include "RequestExecutor.h"
#include "CreateDatabaseCommand.h"
#include "DeleteDatabaseCommand.h"
#include "PutDocumentCommand.h"
#include "GetDocumentsCommand.h"
#include "DeleteDocumentCommand.h"
#include "GetDatabaseNamesCommand.h"

//TODO change vector/concurrency error delDoc test.

namespace
{
	const std::string RAVEN_SERVER_URL = "http://localhost:8080";

	struct User
	{
		std::string FirstName;
		std::string LastName;
	};

	bool operator==(const User& lhs, const User& rhs)
	{
		return lhs.FirstName == rhs.FirstName &&
			lhs.LastName == rhs.LastName;
	}

	void to_json(nlohmann::json& j, const User& u)
	{
		j["FirstName"] = u.FirstName;
		j["LastName"] = u.LastName;
	}

	void from_json(const nlohmann::json& j, User& u)
	{
		u.FirstName = j.at("FirstName").get<std::string>();
		u.LastName = j.at("LastName").get<std::string>();
	}
}

class DocumentCommandsTests : public ::testing::Test
{
	static bool contains_test_prefix(const std::string& str)
	{
		return (str.compare(0, test_prefix.length(), test_prefix) == 0);
	};
public:
	static const std::string db;
	static const std::string test_prefix;
protected:

	static void SetUpTestCase() //make TEST DB(s)
	{
		auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
		ravendb::client::DatabaseRecord rec;
		rec.database_name = db;
		ravendb::client::CreateDatabaseCommand cmd(rec, 1);
		executor->execute<ravendb::client::DatabasePutResult>(cmd);
	}

	static void TearDownTestCase() //delete all TEST DBs
	{
		auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, {});
		ravendb::client::GetDatabaseNamesCommand cmd(0, 100);
		auto res = executor->execute<std::vector<std::string>>(cmd);
		for (const auto& dbname : res)
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

const std::string DocumentCommandsTests::test_prefix{ "TEST__" };
const std::string DocumentCommandsTests::db{ "TEST__DocumentCommandsTests" };

static bool does_document_exist(const std::string& db_name, const std::string& doc_id)
{
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, db_name);
	ravendb::client::GetDocumentsCommand cmd(doc_id, {}, true);
	auto&& res = executor->execute(cmd);

	bool docExists = false;
	for (auto& doc : res.results)
	{
		if (std::string(doc.type_name()) == "null")
		{
			continue;
		}
		if (doc.at("@metadata").at("@id").get<std::string>() == doc_id)
		{
			docExists = true;
			break;
		}
	}
	return docExists;
}

TEST_F(DocumentCommandsTests, CanCreateNewDocument)
{
	User user{ "Alexander","Timoshenko" };
	nlohmann::json j = user;
	std::string newID{ "newID" };
	ravendb::client::PutDocumentCommand cmd(newID, {}, j);

	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, db);

	auto&& res = executor->execute(cmd);
	ASSERT_EQ(res.id, newID);
	ASSERT_FALSE(res.change_vector.empty());
}


TEST_F(DocumentCommandsTests, CanGetDocument)
{
	User user{ "Alexander","Timoshenko" };
	nlohmann::json j = user;
	std::string id{ "newID" };
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, db);
	{
		ravendb::client::PutDocumentCommand cmd(id, {}, j);
		executor->execute(cmd);
	}

	ravendb::client::GetDocumentsCommand cmd(id, {}, false);
	auto&& res = executor->execute(cmd);
	bool doc_exists = false;
	for (auto& doc : res.results)
	{
		if (std::string(doc.type_name()) == "null")
		{
			continue;
		}
		if (doc.at("@metadata").at("@id").get<std::string>() == id)
		{
			User got_user = doc;
			ASSERT_EQ(user, got_user);
			doc_exists = true;
			break;
		}
	}
	ASSERT_TRUE(doc_exists);
}

TEST_F(DocumentCommandsTests, CanDeleteDocument)
{
	User user{ "Alexander","Timoshenko" };
	nlohmann::json j = user;
	std::string id{ "newID123" };
	auto executor = ravendb::client::http::RequestExecutor::create({ RAVEN_SERVER_URL }, db, {});
	{
		ravendb::client::PutDocumentCommand cmd(id, {}, j);
		executor->execute(cmd);
	}
	ASSERT_TRUE(does_document_exist(db, id));

	{
		ravendb::client::DeleteDocumentCommand cmd(id);
		executor->execute(cmd);
	}
	ASSERT_FALSE(does_document_exist(db, id));
}





