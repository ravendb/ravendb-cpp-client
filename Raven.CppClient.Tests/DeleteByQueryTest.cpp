#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "DeleteByQueryOperation.h"

namespace ravendb::client::tests::client::documents::operations
{
	class DeleteByQueryTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(DeleteByQueryTest, CanDeleteByQuery)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->age = 5;
			session.store(user1);

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->age = 10;
			session.store(user2);

			session.save_changes();
		}

		auto index_query = ravendb::client::documents::queries::IndexQuery(
			"from users where age == 5");
		auto operation = ravendb::client::documents::operations::DeleteByQueryOperation(std::move(index_query));
		auto async_op = store->operations()->send_async(operation);

		async_op->wait_for_completion();

		{
			auto session = store->open_session();
			ASSERT_EQ(1,
				session.query<infrastructure::entities::User>()
					->count());
		}
	}

	TEST_F(DeleteByQueryTest, DISABLED_CanDeleteByQueryWaitUsingChanges)
	{
		//TODO waiting for DatabaseChanges implementation
	}
}
