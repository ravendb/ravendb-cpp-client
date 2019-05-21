#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "GetClientConfigurationOperation.h"
#include "PutClientConfigurationOperation.h"
#include "MaintenanceOperationExecutor.h"
#include "PutServerWideClientConfigurationOperation.h"
#include "GetServerWideClientConfigurationOperation.h"

namespace ravendb::client::tests::client::documents::operations::configuration
{
	class ClientConfigurationTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(ClientConfigurationTest, CanSaveAndReadServerWideClientConfiguration)
	{
		//TODO test is OK , yet it changes global definitions -> waiting for embedded
		//auto store = get_document_store(TEST_NAME);

		//auto configuration_to_save = ravendb::client::documents::operations::configuration::ClientConfiguration();
		//configuration_to_save.max_number_of_requests_per_session = 80;
		//configuration_to_save.read_balance_behaviour = http::ReadBalanceBehavior::FASTEST_NODE;
		//configuration_to_save.disabled = true;

		//auto save_operation = serverwide::operations::configuration::
		//	PutServerWideClientConfigurationOperation(std::move(configuration_to_save));
		//store->maintenance()->server()->send(save_operation);

		//auto get_operation = serverwide::operations::configuration::GetServerWideClientConfigurationOperation();
		//auto new_configuration = store->maintenance()->server()->send(get_operation);

		//ASSERT_TRUE(new_configuration);
		//ASSERT_TRUE(new_configuration->disabled);
		//ASSERT_EQ(80, new_configuration->max_number_of_requests_per_session);
		//ASSERT_EQ(http::ReadBalanceBehavior::FASTEST_NODE, new_configuration->read_balance_behaviour);
	}

	TEST_F(ClientConfigurationTest, CanHandleNoConfiguration)
	{
		auto store = get_document_store(TEST_NAME);

		auto operation = ravendb::client::documents::operations::configuration::GetClientConfigurationOperation();
		auto result = store->maintenance()->send(operation);

		ASSERT_FALSE(result->configuration.has_value());
		ASSERT_GE(result->etag, 0);
	}

	TEST_F(ClientConfigurationTest, CanSaveAndReadClientConfiguration)
	{
		auto store = get_document_store(TEST_NAME);

		auto configuration_to_save = ravendb::client::documents::operations::configuration::ClientConfiguration();
		configuration_to_save.etag = 123L;
		configuration_to_save.max_number_of_requests_per_session = 80;
		configuration_to_save.read_balance_behaviour = http::ReadBalanceBehavior::FASTEST_NODE;
		configuration_to_save.disabled = true;

		auto save_operation = 
			ravendb::client::documents::operations::configuration::PutClientConfigurationOperation(configuration_to_save);
		store->maintenance()->send(save_operation);
		
		auto operation = ravendb::client::documents::operations::configuration::GetClientConfigurationOperation();
		auto result = store->maintenance()->send(operation);

		ASSERT_NE(result->etag, 0);

		auto new_configuration = result->configuration;

		ASSERT_TRUE(new_configuration.has_value());
		ASSERT_GT(new_configuration->etag, configuration_to_save.etag);
		ASSERT_TRUE(new_configuration->disabled);
		ASSERT_EQ(80, new_configuration->max_number_of_requests_per_session);
		ASSERT_EQ(http::ReadBalanceBehavior::FASTEST_NODE, new_configuration->read_balance_behaviour);
	}
}
