#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "GetClusterTopologyCommand.h"

namespace ravendb::client::tests::serverwide_commands
{
	class GetClusterTopologyTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(GetClusterTopologyTest, CanGetTopology)
	{
		auto store = get_document_store(TEST_NAME);
		
		auto command = serverwide::commands::GetClusterTopologyCommand();

		store->get_request_executor()->execute(command);

		const auto& result = command.get_result();

		ASSERT_TRUE(result);
		ASSERT_FALSE(result->leader.empty());
		ASSERT_FALSE(result->node_tag.empty());

		const auto& topology = result->topology;

		ASSERT_FALSE(topology.topology_id.empty());
		ASSERT_FALSE(topology.members.empty());
		ASSERT_TRUE(topology.watchers.empty());
		ASSERT_TRUE(topology.promotables.empty());
	}
}
