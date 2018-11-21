#include "pch.h"
#include "definitions.h"

#include "User.h"
#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "DeleteDocumentCommand.h"
#include "GetNextOperationIdCommand.h"

//TODO change_vector/concurrency error delDoc test.

using namespace ravendb::client;

class DocumentCommandsTests : public ::testing::Test
{
protected:
	inline static std::unique_ptr<RequestExecutorScope> test_suite_executor{};

	static const User example_user;

	static void SetUpTestCase()
	{
		test_suite_executor = GET_REQUEST_EXECUTOR();
	}

	void SetUp() override//create sample document
	{
		nlohmann::json j = example_user;
		std::string tsdfsdf = j.dump();
		documents::commands::PutDocumentCommand cmd(example_user.id, {}, j);
		test_suite_executor->get()->execute(cmd);
	}

	static bool does_document_exist(const std::string& _db_name, const std::string& doc_id)
	{
		auto _executor = get_raw_request_executor(_db_name);
		documents::commands::GetDocumentsCommand cmd(doc_id, {}, true);
		auto&& res = _executor->execute(cmd);

		return !res.results.empty() && (std::strcmp(res.results[0].type_name(), "null") != 0);
	}
};

const User DocumentCommandsTests::example_user{ "users/1-A", "Alexander", "Timoshenko", "Israel", 0, 38 };


TEST_F(DocumentCommandsTests, CanGetDocument)
{
	ASSERT_TRUE(does_document_exist(test_suite_executor->get_db_name(), example_user.id));

 	documents::commands::GetDocumentsCommand cmd(example_user.id, {}, false);
	auto&& res = test_suite_executor->get()->execute(cmd);

	User check_user = res.results[0];
	ASSERT_EQ(example_user, check_user);
}

TEST_F(DocumentCommandsTests, CanDeleteDocument)
{
	ASSERT_TRUE(does_document_exist(test_suite_executor->get_db_name(), example_user.id));

	{
		documents::commands::DeleteDocumentCommand cmd(example_user.id);
		test_suite_executor->get()->execute(cmd);
	}
	ASSERT_FALSE(does_document_exist(test_suite_executor->get_db_name(), example_user.id));
}

TEST_F(DocumentCommandsTests, CanGetNextOperationId)
{
	documents::commands::GetNextOperationIdCommand cmd{};
	auto&& res = test_suite_executor->get()->execute(cmd);

	ASSERT_GE(res, 0);
}
