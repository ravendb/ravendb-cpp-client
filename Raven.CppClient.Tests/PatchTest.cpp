#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"
#include "AdvancedSessionOperations.h"
#include "PatchByQueryOperation.h"
#include "JavaScriptException.h"

namespace ravendb::client::tests::client
{
	class PatchTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::User, id);
		}

		class Users_ByName : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Users_ByName() override = default;
			Users_ByName()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from u in docs.Users select new { u.name }";

				index("name", documents::indexes::FieldIndexing::SEARCH);

				index_suggestions.insert("name");

				store("name", documents::indexes::FieldStorage::YES);
			}
		};
	};

	TEST_F(PatchTest, CanPatchSingleDocument)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";
			session.store(user, "users/1");			
			session.save_changes();
		}

		auto patch_operation = documents::operations::PatchOperation("users/1", {},
			documents::operations::PatchRequest("this.name = \"Patched\""));
		auto status = store->operations()->send(patch_operation);

		ASSERT_EQ(documents::operations::PatchStatus::PATCHED, status);

		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			ASSERT_EQ("Patched", user->name);
		}
	}

	TEST_F(PatchTest, CanWaitForIndexAfterPatch)
	{
		auto store = get_document_store(TEST_NAME);

		Users_ByName().execute(store);

		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.advanced().wait_for_indexes_after_save_changes(
				[](auto& opts) {opts.wait_for_indexes({ "Users/ByName" }); });

			auto user = session.load<infrastructure::entities::User>("users/1");
			session.advanced().patch(user, "name", "New Name");
			session.save_changes();
		}
	}

	TEST_F(PatchTest, CanPatchManyDocuments)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();

			ASSERT_EQ(1,
				session.query<infrastructure::entities::User>()
					->count_lazily().get_value());
		}

		auto operation = documents::operations::PatchByQueryOperation("from Users update {  this.name= \"Patched\"  }");

		auto op = store->operations()->send_async(operation);

		op->wait_for_completion();

		{
			auto session = store->open_session();
			auto loaded_user = session.load<infrastructure::entities::User>("users/1");

			ASSERT_EQ("Patched", loaded_user->name);
		}
	}

	TEST_F(PatchTest, ThrowsOnInvalidScript)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "RavenDB";

			session.store(user, "users/1");
			session.save_changes();

			ASSERT_EQ(1,
				session.query<infrastructure::entities::User>()
				->count_lazily().get_value());
		}

		auto operation = documents::operations::PatchByQueryOperation("from Users update {  throw 5 }");

		auto op = store->operations()->send_async(operation);

		ASSERT_THROW(op->wait_for_completion(), ravendb::client::exceptions::documents::patching::JavaScriptException);		
	}
}
