#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"
#include "AdvancedSessionOperations.h"
#include "PatchByQueryOperation.h"

namespace advanced_patching_test
{
	struct CustomType
	{
		std::string id{};
		std::string owner{};
		int32_t value{};
		std::vector<std::string> comments{};
		ravendb::client::impl::DateTimeOffset date{};
	};

	void to_json(nlohmann::json& j, const CustomType& ct)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "owner", ct.owner);
		set_val_to_json(j, "value", ct.value);
		set_val_to_json(j, "comments", ct.comments);
		set_val_to_json(j, "date", ct.date);
	}

	void from_json(const nlohmann::json& j, CustomType& ct)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "owner", ct.owner);
		get_val_from_json(j, "value", ct.value);
		get_val_from_json(j, "comments", ct.comments);
		get_val_from_json(j, "date", ct.date);
	}
}

namespace ravendb::client::tests::server::patching
{
	class AdvancedPatchingTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(advanced_patching_test::CustomType, id);
		}

		static constexpr char SAMPLE_SCRIPT[] = "this.comments.splice(2, 1);\n"
			"    this.owner = 'Something new';\n"
			"    this.value++;\n"
			"    this.newValue = \"err!!\";\n"
			"    this.comments = this.comments.map(function(comment) {\n"
			"        return (comment == \"one\") ? comment + \" test\" : comment;\n"
			"    });";
	};

	TEST_F(AdvancedPatchingTest, TestWithVariables)
	{
		using namespace advanced_patching_test;

		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto custom_type = std::make_shared<CustomType>();
			custom_type->owner = "me";
			session.store(custom_type, "customTypes/1");
			session.save_changes();
		}

		auto patch_request = documents::operations::PatchRequest();
		patch_request.script = "this.owner = args.v1";
		patch_request.values.insert_or_assign("v1", "not-me");

		auto patch_operation = documents::operations::PatchOperation("customTypes/1", {}, patch_request);
		store->operations()->send(patch_operation);

		{
			auto session = store->open_session();
			auto loaded = session.load<CustomType>("customTypes/1");
			ASSERT_EQ("not-me", loaded->owner);
		}
	}

	TEST_F(AdvancedPatchingTest, CanCreateDocumentsIfPatchingAppliedByIndex)
	{
		using namespace advanced_patching_test;

		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto type1 = std::make_shared<CustomType>();
			type1->id = "Item/1";
			type1->value = 1;
			session.store(type1);

			auto type2 = std::make_shared<CustomType>();
			type2->id = "Item/2";
			type2->value = 2;
			session.store(type2);

			session.save_changes();
		}

		auto def1 = documents::indexes::IndexDefinition();
		def1.name = "TestIndex";
		def1.maps.insert("from doc in docs.CustomTypes select new { doc.value }");

		store->maintenance()->send(documents::operations::indexes::PutIndexesOperation({ def1 }));

		{
			auto session = store->open_session();

			session.advanced().document_query<CustomType>("TestIndex", {}, false)
				->wait_for_non_stale_results()
				->to_list();
		}
		auto operation = store->operations()->send_async(documents::operations::PatchByQueryOperation(
			"FROM INDEX 'TestIndex' WHERE value = 1 update { put('NewItem/3', {'copiedValue': this.value });}"));
		operation->wait_for_completion();
		{
			auto session = store->open_session();
			auto json_document = session.load<nlohmann::json>("NewItem/3");
			ASSERT_EQ(1, json_document->at("copiedValue").get<int32_t>());
		}
	}


	TEST_F(AdvancedPatchingTest, CanApplyBasicScriptAsPatch)
	{
		using namespace advanced_patching_test;

		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto test = std::make_shared<CustomType>();
			test->id = "someId";
			test->owner = "bob";
			test->value = 12143;
			test->comments.assign({ "one", "two", "seven" });

			session.store(test);
			session.save_changes();
		}
		store->operations()->send(documents::operations::PatchOperation("someId",
			{}, documents::operations::PatchRequest(SAMPLE_SCRIPT)));
		{
			auto session = store->open_session();

			auto result = session.load<CustomType>("someId");

			ASSERT_EQ("Something new", result->owner);
			ASSERT_EQ(2, result->comments.size());
			ASSERT_EQ("one test", result->comments.at(0));
			ASSERT_EQ("two", result->comments.at(1));
			ASSERT_EQ(12144, result->value);
		}
	}

	TEST_F(AdvancedPatchingTest, CanDeserializeModifiedDocument)
	{
		using namespace advanced_patching_test;

		auto store = get_document_store(TEST_NAME);

		auto custom_type = std::make_shared<CustomType>();
		custom_type->owner = "somebody@somewhere.com";
		{
			auto session = store->open_session();
			session.store(custom_type, "doc");
			session.save_changes();
		}

		auto patch1 = documents::operations::PatchOperation("doc", {},
			documents::operations::PatchRequest("this.owner = '123';"));
		auto result = store->operations()->send<CustomType>(patch1);

		ASSERT_EQ(documents::operations::PatchStatus::PATCHED, result.first);
		ASSERT_EQ("123", result.second.owner);

		auto patch2 = documents::operations::PatchOperation("doc", {},
			documents::operations::PatchRequest("this.owner = '123';"));
		result = store->operations()->send<CustomType>(patch2);

		ASSERT_EQ(documents::operations::PatchStatus::NOT_MODIFIED, result.first);
		ASSERT_EQ("123", result.second.owner);
	}

}
