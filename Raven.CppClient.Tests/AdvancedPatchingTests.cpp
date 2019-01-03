#include "pch.h"
#include "definitions.h"
#include "IDocumentStore.h"
#include "PutDocumentCommand.h"
#include "GetDocumentsCommand.h"
#include "DeleteDocumentCommand.h"
#include "DateTimeOffset.h"
#include "PatchOperation.h"

namespace
{
	class FakeStore : public ravendb::client::documents::IDocumentStore
	{};
}

namespace ravendb::client::tests
{
	namespace adv_patching_tests
	{
		struct CustomType
		{
			std::string id{};
			std::string owner{};
			int32_t value{};
			std::vector<std::string> comments{};
			impl::DateTimeOffset date{};
		};

		inline void to_json(nlohmann::json& j, const CustomType& ct)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "Id", ct.id);
			set_val_to_json(j, "Owner", ct.owner);
			set_val_to_json(j, "Comments", ct.comments);
			set_val_to_json(j, "Date", ct.date);
			j["@metadata"]["@collection"] = "CustomTypes";
		}

		inline void from_json(const nlohmann::json& j, CustomType& ct)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "Id", ct.id);
			get_val_from_json(j, "Owner", ct.owner);
			get_val_from_json(j, "Comments", ct.comments);
			get_val_from_json(j, "Date", ct.date);
		}
	}

	class AdvancedPatchingTests : public ::testing::Test
	{
	protected:
		inline static std::unique_ptr<RequestExecutorScope> test_suite_executor{};

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
		}

		void TearDown() override
		{
			auto  get_docs_cmd = documents::commands::GetDocumentsCommand({}, {}, {}, {}, 0, 100, true);
			auto results = test_suite_executor->get()->execute(get_docs_cmd);
			for (const auto& res : results.results)
			{
				auto del_doc_cmd = documents::commands::DeleteDocumentCommand(res["@metadata"]["@id"].get<std::string>());
				test_suite_executor->get()->execute(del_doc_cmd);
			}
		}
	};

	TEST_F(AdvancedPatchingTests, TestWithVariables)
	{
		using namespace adv_patching_tests;
		auto custom_type = CustomType{ "CustomTypes/1","me" };
		auto put_doc_cmd = documents::commands::PutDocumentCommand(custom_type.id, {}, custom_type);
		auto&& res1 = test_suite_executor->get()->execute(put_doc_cmd);
		ASSERT_EQ(res1.id, custom_type.id);

		auto patch = documents::operations::PatchRequest("this.Owner = args.v1");
		patch.values = { {"v1","someone else"} };
		auto op = documents::operations::PatchOperation(custom_type.id, {}, patch);

		HttpCache cache;
		auto&& res2 = test_suite_executor->get()->execute(op.get_command(FakeStore(), {}, cache));

		ASSERT_EQ(res2.status, documents::operations::PatchStatus::PATCHED);

		auto get_doc_cmd = documents::commands::GetDocumentsCommand(custom_type.id, {}, false);
		auto&& res3 = test_suite_executor->get()->execute(get_doc_cmd);

		CustomType ret_custom_type = res3.results[0];
		ASSERT_EQ(ret_custom_type.owner, "someone else");
	}

}
