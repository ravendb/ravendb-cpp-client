#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "IndexDefinition.h"

namespace ravendb::client::tests::client::indexing
{
	class DetectIndexTypeTest : public driver::RavenTestDriver
	{
	protected:
		static documents::indexes::IndexType find_index_type(const std::string& map)
		{
			auto index_definition = documents::indexes::IndexDefinition();
			index_definition.maps.insert(map);

			return index_definition.get_type();
		}
	};

	TEST_F(DetectIndexTypeTest, TestValidMap)
	{
		std::string map{ "from task in docs.Tasks select new { task.assigneeId }" };

		ASSERT_EQ(documents::indexes::IndexType::MAP, find_index_type(map));
	}

	TEST_F(DetectIndexTypeTest, TestMapSpaces)
	{
		std::string map{ "   from task in docs.Tasks select new { task.assigneeId }" };

		ASSERT_EQ(documents::indexes::IndexType::MAP, find_index_type(map));
	}

	TEST_F(DetectIndexTypeTest, TestMapTabsBreaks)
	{
		std::string map{ " \r\r  \r\n  from task in docs.Tasks select new { task.assigneeId }" };

		ASSERT_EQ(documents::indexes::IndexType::MAP, find_index_type(map));
	}

	TEST_F(DetectIndexTypeTest, TestMapComments)
	{
		std::string map{ "// this is comment \r\n from task in docs.Tasks select new { task.assigneeId }" };

		ASSERT_EQ(documents::indexes::IndexType::MAP, find_index_type(map));
	}

	TEST_F(DetectIndexTypeTest, TestJsMap)
	{
		std::string map{ "map('Users', x => x)" };

		ASSERT_EQ(documents::indexes::IndexType::JAVASCRIPT_MAP, find_index_type(map));
	}

	TEST_F(DetectIndexTypeTest, TestJsMapComment)
	{
		std::string map{ "//this is test\r\n  map('Users', x => x)" };

		ASSERT_EQ(documents::indexes::IndexType::JAVASCRIPT_MAP, find_index_type(map));
	}
}
