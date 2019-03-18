#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "RequestExecutor.h"
#include "GetNextOperationIdCommand.h"

namespace ravendb::client::tests::client::documents::commands
{
	class GetNextOperationIdCommandTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(GetNextOperationIdCommandTest, CanGetNextOperationId)
	{
		auto store = get_document_store(TEST_NAME);

		auto command = ravendb::client::documents::commands::GetNextOperationIdCommand();
		store->get_request_executor()->execute(command);

		ASSERT_GE(*command.get_result(), 0);
	}
}
