#include "pch.h"
#include "re_definitions.h"

#include "User.h"
#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "DeleteDocumentCommand.h"

namespace ravendb::client::tests
{
	class SecuredRequestExecutorTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<definitions::RequestExecutorScope> test_suite_executor{};

		static const infrastructure::entities::User example_user;

		static void SetUpTestCase()
		{
			test_suite_executor = definitions::GET_SECURED_REQUEST_EXECUTOR();
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

	const infrastructure::entities::User SecuredRequestExecutorTests::example_user{ "users/1-A", "Alexander", "Timoshenko", "Israel", 0, 38 };


	TEST_F(SecuredRequestExecutorTests, CanGetDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::GetDocumentsCommand cmd(example_user.id, {}, false);
		auto&& res = test_suite_executor->get().execute(cmd);

		infrastructure::entities::User check_user = res.results[0].get<infrastructure::entities::User>();
		ASSERT_EQ(example_user, check_user);
	}

	TEST_F(SecuredRequestExecutorTests, CanDeleteDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::DeleteDocumentCommand cmd(example_user.id);
		test_suite_executor->get().execute(cmd);

		ASSERT_FALSE(does_document_exist(example_user.id));
	}
}
