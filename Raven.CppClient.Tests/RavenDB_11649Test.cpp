#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::tests::issues
{
	namespace RavenDB_11649Test_ns
	{
		struct InnerClass
		{
			std::string a{};
			std::optional<std::string> aNullable{};
		};

		void from_json(const nlohmann::json& j, InnerClass& ic)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "a", ic.a);
			get_val_from_json(j, "aNullable", ic.aNullable);
		}

		void to_json(nlohmann::json& j, const InnerClass& ic)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "a", ic.a);
			set_val_to_json(j, "aNullable", ic.aNullable);
		}

		struct MiddleClass
		{
			std::optional<InnerClass> a{};
		};

		void from_json(const nlohmann::json& j, MiddleClass& mc)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "a", mc.a);
		}

		void to_json(nlohmann::json& j, const MiddleClass& mc)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "a", mc.a);
		}

		struct OuterClass
		{
			std::vector<std::vector<InnerClass>> innerClassMatrix{};
			std::vector<InnerClass> innerClasses{};
			std::string a{};
			InnerClass innerClass{};
			MiddleClass middleClass{};
		};

		void from_json(const nlohmann::json& j, OuterClass& oc)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "innerClassMatrix", oc.innerClassMatrix);
			get_val_from_json(j, "innerClasses", oc.innerClasses);
			get_val_from_json(j, "a", oc.a);
			get_val_from_json(j, "innerClass", oc.innerClass);
			get_val_from_json(j, "middleClass", oc.middleClass);
		}

		void to_json(nlohmann::json& j, const OuterClass& oc)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "innerClassMatrix", oc.innerClassMatrix);
			set_val_to_json(j, "innerClasses", oc.innerClasses);
			set_val_to_json(j, "a", oc.a);
			set_val_to_json(j, "innerClass", oc.innerClass);
			set_val_to_json(j, "middleClass", oc.middleClass);
		}
	}

	class RavenDB_11649Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RavenDB_11649Test, WhatChanged_WhenInnerPropertyChanged_ShouldReturnThePropertyNamePlusPath)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
		doc->a = "outerValue";

		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->innerClass.a = "newInnerValue";

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClass");
	}

	TEST_F(RavenDB_11649Test, WhatChanged_WhenInnerPropertyChangedFromNull_ShouldReturnThePropertyNamePlusPath)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
		doc->a = "outerValue";

		doc->innerClass.aNullable.reset();

		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->innerClass.aNullable.emplace("newInnerValue");

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClass");
	}

	TEST_F(RavenDB_11649Test, WhatChanged_WhenPropertyOfInnerPropertyChangedToNull_ShouldReturnThePropertyNamePlusPath)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
		doc->a = "outerValue";

		doc->innerClass.aNullable.emplace("innerValue");

		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->innerClass.aNullable.reset();

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClass");
	}

	TEST_F(RavenDB_11649Test, WhatChanged_WhenOuterPropertyChanged_FieldPathShouldBeEmpty)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
		doc->a = "outerValue";

		doc->innerClass.a = "innerValue";

		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->a = "newOuterValue";

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0).empty());
	}

	////For now commented because the compiler crashes when I try to compile this test
	//TEST_F(RavenDB_11649Test, WhatChanged_WhenInnerPropertyInArrayChanged_ShouldReturnWithRelevantPath)
	//{
	//	auto store = get_document_store(TEST_NAME);

	//	auto session = store->open_session();
	//	// arrange
	//	auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
	//	doc->a = "outerValue";

	//	RavenDB_11649Test_ns::InnerClass&& inner_class = RavenDB_11649Test_ns::InnerClass{};
	//	inner_class.a = "innerValue";
	//	doc->innerClasses.emplace_back(std::move(inner_class));

	//	const auto id = std::string{ "docs/1" };
	//	session.store(doc, id);
	//	session.save_changes();

	//	doc->innerClasses.at(0).a = "newInnerValue";

	//	// action
	//	auto&& changes = session.advanced().what_changed();

	//	// assert
	//	std::vector<std::string> changes_paths{};
	//	auto&& changes_for_id = changes.at(id);
	//	std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
	//		[](const documents::session::DocumentsChanges& c)
	//	{
	//		return c.field_path;
	//	});

	//	ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClasses[0]");
	//}

	TEST_F(RavenDB_11649Test, WhatChanged_WhenArrayPropertyInArrayChangedFromNull_ShouldReturnWithRelevantPath)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();

		doc->innerClassMatrix.emplace_back();
		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->innerClassMatrix.at(0).emplace_back();

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClassMatrix[0]");
	}

	TEST_F(RavenDB_11649Test, WhatChanged_WhenInMatrixChanged_ShouldReturnWithRelevantPath)
	{
		auto store = get_document_store(TEST_NAME);

		auto session = store->open_session();
		// arrange
		auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
		doc->innerClassMatrix.emplace_back();
		doc->innerClassMatrix.at(0).emplace_back();
		doc->innerClassMatrix.at(0).at(0).a = "oldValue";

		const auto id = std::string{ "docs/1" };
		session.store(doc, id);
		session.save_changes();

		doc->innerClassMatrix[0][0].a = "newValue";

		// action
		auto&& changes = session.advanced().what_changed();

		// assert
		std::vector<std::string> changes_paths{};
		auto&& changes_for_id = changes.at(id);
		std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
			[](const documents::session::DocumentsChanges& c)
		{
			return c.field_path;
		});

		ASSERT_TRUE(changes_paths.size() == 1 && changes_paths.at(0) == "innerClassMatrix[0][0]");
	}

	////For now commented because the compiler crashes when I try to compile this test
	//TEST_F(RavenDB_11649Test, WhatChanged_WhenAllNamedAPropertiesChanged_ShouldReturnDifferentPaths)
	//{
	//	auto store = get_document_store(TEST_NAME);

	//	auto session = store->open_session();
	//	// arrange
	//	auto doc = std::make_shared<RavenDB_11649Test_ns::OuterClass>();
	//	doc->a = "outerValue";

	//	doc->innerClass.a = "innerValue";

	//	auto&& inner_class2 = RavenDB_11649Test_ns::InnerClass{};
	//	inner_class2.a = "oldValue";
	//	doc->innerClasses.emplace_back(std::move(inner_class2));

	//	doc->innerClassMatrix.emplace_back();
	//	doc->innerClassMatrix.at(0).emplace_back();
	//	doc->innerClassMatrix.at(0).at(0).a = "oldValue";

	//	const auto id = std::string{ "docs/1" };
	//	session.store(doc, id);
	//	session.save_changes();

	//	doc->a = "newOuterValue";
	//	doc->innerClass.a = "newInnerValue";
	//	doc->middleClass.a.emplace();
	//	doc->innerClasses[0].a = "newValue";
	//	doc->innerClassMatrix[0][0].a = "newValue";

	//	// action
	//	auto&& changes = session.advanced().what_changed();

	//	// assert
	//	std::vector<std::string> changes_paths{};
	//	auto&& changes_for_id = changes.at(id);
	//	std::transform(changes_for_id.cbegin(), changes_for_id.cend(), std::back_inserter(changes_paths),
	//		[](const documents::session::DocumentsChanges& c)
	//	{
	//		return c.field_path;
	//	});

	//	ASSERT_EQ(5, changes_paths.size());
	//	
	//	const auto changes_paths_contains = [&](const std::string& path)
	//	{
	//		return std::find(changes_paths.cbegin(), changes_paths.cend(), path) != changes_paths.cend();
	//	};
	//	ASSERT_TRUE(changes_paths_contains(""));
	//	ASSERT_TRUE(changes_paths_contains("innerClass"));
	//	ASSERT_TRUE(changes_paths_contains("middleClass"));
	//	ASSERT_TRUE(changes_paths_contains("innerClasses[0]"));
	//	ASSERT_TRUE(changes_paths_contains("innerClassMatrix[0][0]"));
	//}
}
