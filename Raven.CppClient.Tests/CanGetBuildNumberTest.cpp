#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "MaintenanceOperationExecutor.h"
#include "GetBuildNumberOperation.h"

namespace ravendb::client::tests::client::documents::commands
{
	class CanGetBuildNumberTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(CanGetBuildNumberTest, CanGetBuildNumber)
	{
		auto store = get_document_store(TEST_NAME);

		auto build_number = store->maintenance()->server()->send(serverwide::operations::GetBuildNumberOperation());

		ASSERT_TRUE(build_number);
		ASSERT_FALSE(build_number->product_version.empty());
	}
}
