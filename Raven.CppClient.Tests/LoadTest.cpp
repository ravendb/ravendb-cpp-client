#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"

namespace load_test
{
	struct Foo
	{
		std::string name{};
	};

	inline void to_json(nlohmann::json& j, const Foo& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", f.name);
	}

	inline void from_json(const nlohmann::json& j, Foo& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", f.name);
	}

	struct Bar
	{
		std::string foo_id{};
		std::vector<std::string> foo_ids{};
		std::string name{};
	};


	inline void to_json(nlohmann::json& j, const Bar& b)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "FooId", b.foo_id);
		set_val_to_json(j, "FooIds", b.foo_ids);
		set_val_to_json(j, "Name", b.name);
	}

	inline void from_json(const nlohmann::json& j, Bar& b)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "FooId", b.foo_id);
		get_val_from_json(j, "FooIds", b.foo_ids);
		get_val_from_json(j, "Name", b.name);
	}
}

namespace ravendb::client::tests::client::documents
{
	class LoadTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(LoadTest, LoadWithIncludes)
	{
		std::string bar_id{};
		{
			auto session = test_suite_store->get()->open_session();
			auto foo = std::make_shared<load_test::Foo>();
			foo->name = "Beginning";
			session.store(foo);

			auto foo_id = *session.advanced().get_document_id(foo);
			auto bar = std::make_shared<load_test::Bar>();
			bar->name = "End";
			bar->foo_id = foo_id;
			session.store(bar);

			bar_id = *session.advanced().get_document_id(bar);
			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();
			std::vector<std::string> temp_ids = { bar_id };
			auto bar = session.include("FooId").load<load_test::Bar>(temp_ids.begin(), temp_ids.end());

			ASSERT_EQ(1, bar.size());
			ASSERT_TRUE(bar.find(bar_id) != bar.end());

			auto num_of_requests = session.advanced().get_number_of_requests();

			auto foo = session.load<load_test::Foo>(bar.at(bar_id)->foo_id);

			ASSERT_TRUE(foo);
			ASSERT_EQ(std::string("Beginning"), foo->name);
			ASSERT_EQ(num_of_requests, session.advanced().get_number_of_requests());
		}
	}

	//TODO this test MAY be influenced from non-implemented feature(IncludesUtils = include path parsing)
	TEST_F(LoadTest, LoadWithIncludesAndMissingDocument)
	{
		std::string bar_id{};
		{
			auto session = test_suite_store->get()->open_session();
			auto bar = std::make_shared<load_test::Bar>();
			bar->name = "End";
			bar->foo_id = "non_exists/1";
			session.store(bar);
			bar_id = *session.advanced().get_document_id(bar);
			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();
			std::vector<std::string> temp_ids = { bar_id };
			auto bar = session.include("FooId").load<load_test::Bar>(temp_ids.begin(), temp_ids.end());

			ASSERT_EQ(1, bar.size());
			ASSERT_TRUE(bar.find(bar_id) != bar.end());

			auto num_of_requests = session.advanced().get_number_of_requests();

			auto foo = session.load<load_test::Foo>(bar.at(bar_id)->foo_id);

			ASSERT_FALSE(foo);
			ASSERT_EQ(num_of_requests, session.advanced().get_number_of_requests());
		}
	}
}

