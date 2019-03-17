#include "pch.h"
//#define __USE_FIDDLER__
#include "re_definitions.h"
#include "User.h"
#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "DeleteDocumentCommand.h"
#include "GetNextOperationIdCommand.h"

namespace ravendb::client::tests::old_tests
{
	class BasicDocumentCommandsTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<definitions::RequestExecutorScope> test_suite_executor{};

		static const infrastructure::entities::User example_user;

		static void SetUpTestCase()
		{
			test_suite_executor = definitions::GET_REQUEST_EXECUTOR();
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}

		void SetUp() override//create sample document
		{
			nlohmann::json j = example_user;
			documents::commands::PutDocumentCommand cmd(example_user.id, {}, j);
			test_suite_executor->get().execute(cmd);
		}

		void TearDown() override //delete sample document
		{
			documents::commands::DeleteDocumentCommand cmd(example_user.id);
			test_suite_executor->get().execute(cmd);
		}

		bool does_document_exist(const std::string& doc_id)
		{
			documents::commands::GetDocumentsCommand cmd(doc_id, {}, true);
			auto&& res = test_suite_executor->get().execute(cmd);

			return !res.results.empty() && !res.results[0].is_null();
		}
	};

	const infrastructure::entities::User BasicDocumentCommandsTests::example_user{ "users/1-A", "Alexander", "Timoshenko", "Israel", 0, 38 };


	TEST_F(BasicDocumentCommandsTests, CanGetDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::GetDocumentsCommand cmd(example_user.id, {}, false);
		auto&& res = test_suite_executor->get().execute(cmd);

		infrastructure::entities::User check_user = res.results[0].get<infrastructure::entities::User>();
		ASSERT_EQ(example_user, check_user);
	}

	TEST_F(BasicDocumentCommandsTests, CanDeleteDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::DeleteDocumentCommand cmd(example_user.id);
		test_suite_executor->get().execute(cmd);

		ASSERT_FALSE(does_document_exist(example_user.id));
	}

	TEST_F(BasicDocumentCommandsTests, ThrowOnWrongChangeVectorDeleteDocumentCommand)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::GetDocumentsCommand cmd1(example_user.id, {}, true);
		auto&& res = test_suite_executor->get().execute(cmd1);
		auto&& change_vector = res.results[0].at("@metadata").at("@change-vector").get<std::string>();

		std::string wrong_change_vector(change_vector.crbegin(), change_vector.crend());
		documents::commands::DeleteDocumentCommand cmd2(example_user.id, wrong_change_vector);
		try
		{
			test_suite_executor->get().execute(cmd2);
		}
		catch (ravendb::client::RavenError& ex)
		{
			EXPECT_NE(std::string(ex.what()).find("ConcurrencyException"), std::string::npos);
			ASSERT_TRUE(does_document_exist(example_user.id));
			SUCCEED();
			return;
		}
		FAIL();
	}

	TEST_F(BasicDocumentCommandsTests, CanGetNextOperationId)
	{
		documents::commands::GetNextOperationIdCommand cmd{};
		auto&& res = test_suite_executor->get().execute(cmd);

		ASSERT_GE(res, 0);
	}
}
