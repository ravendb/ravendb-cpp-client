#pragma once
#include "ds_definitions.h"
#include "test_utils.h"

namespace ravendb::client::tests::infrastructure
{
	class TestSuiteBase : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<definitions::DocumentStoreScope> test_suite_store{};

		static void TearDownTestCase()
		{
			test_suite_store.reset();
		}

		void TearDown() override
		{
			utils::delete_all_documents(test_suite_store);
		}
	};
}
