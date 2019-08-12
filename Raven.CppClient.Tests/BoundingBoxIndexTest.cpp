#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"

namespace bounding_box_index_test
{
	struct SpatialDoc
	{
		std::string id{};
		std::string shape{};
	};

	void from_json(const nlohmann::json& j, SpatialDoc& sd)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "shape", sd.shape);
	}

	void to_json(nlohmann::json& j, const SpatialDoc& sd)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "shape", sd.shape);
	}

	class BBoxIndex : public ravendb::client::documents::indexes::AbstractIndexCreationTask
	{
	public:
		~BBoxIndex() override = default;
		BBoxIndex()
		{
			SET_DEFAULT_INDEX_NAME();

			map = R"(
				docs.SpatialDocs.Select(doc => new {
					shape = this.CreateSpatialField(doc.shape)
				}))";
			spatial("shape", 
				[](const ravendb::client::documents::indexes::spatial::SpatialOptionsFactory& options_factory)->
					ravendb::client::documents::indexes::spatial::SpatialOptions
			{
				return options_factory.cartesian().bounding_box_index();
			});
		}
	};

	class QuadTreeIndex : public ravendb::client::documents::indexes::AbstractIndexCreationTask
	{
	public:
		~QuadTreeIndex() override = default;
		QuadTreeIndex()
		{
			SET_DEFAULT_INDEX_NAME();

			map = R"(
				docs.SpatialDocs.Select(doc => new {
					shape = this.CreateSpatialField(doc.shape)
                }))";
			spatial("shape",
				[](const ravendb::client::documents::indexes::spatial::SpatialOptionsFactory& options_factory)->
				ravendb::client::documents::indexes::spatial::SpatialOptions
			{
				return options_factory.cartesian().quad_prefix_tree_index(6,
					ravendb::client::documents::indexes::spatial::SpatialOptionsFactory::SpatialBounds(0, 0, 16, 16));
			});
		}
	};
}


namespace ravendb::client::tests::spatial
{
	class BoundingBoxIndexTest : public driver::RavenTestDriver
	{
	protected:
		std::shared_ptr<documents::DocumentStore> store;
		std::string rectangle1;
		std::string rectangle2;
		std::string rectangle3;
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(bounding_box_index_test::SpatialDoc, id);
		}

		void SetUp() override {
			std::string polygon = "POLYGON ((0 0, 0 5, 1 5, 1 1, 5 1, 5 5, 6 5, 6 0, 0 0))";
			rectangle1 = "2 2 4 4";
			rectangle2 = "6 6 10 10";
			rectangle3 = "0 0 6 6";
			store = get_document_store(TEST_NAME);
			bounding_box_index_test::BBoxIndex().execute(store);
			bounding_box_index_test::QuadTreeIndex().execute(store);
			{
				auto session = store->open_session();
				auto doc = std::make_shared<bounding_box_index_test::SpatialDoc>();
				doc->shape = polygon;
				session.store(doc);
				session.save_changes();
			}

			wait_for_indexing(store);
		}
	};

	TEST_F(BoundingBoxIndexTest, SimpleQuery)
	{
		auto session = store->open_session();
		auto result = session.query<bounding_box_index_test::SpatialDoc>()->count();
		ASSERT_EQ(1, result);
	}

	TEST_F(BoundingBoxIndexTest, IntersectTest_Rectangle1)
	{
			auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::BBoxIndex>()
				->spatial("shape", 
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
						std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.intersect(rectangle1);
			})
				->count();
			ASSERT_EQ(1, result);

	}

	TEST_F(BoundingBoxIndexTest, IntersectTest_Rectangle2)
	{
			auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::BBoxIndex>()
				->spatial("shape",
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
					std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.intersect(rectangle2);
			})
				->count();
			ASSERT_EQ(0, result);
	}

	TEST_F(BoundingBoxIndexTest, DisjointTest_Rectangle1)
	{
			auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::BBoxIndex>()
				->spatial("shape",
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
					std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.disjoint(rectangle1);
			})
				->count();
			ASSERT_EQ(0, result);
	}

	TEST_F(BoundingBoxIndexTest, DisjointTest_Rectangle2)
	{
			auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::BBoxIndex>()
				->spatial("shape",
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
					std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.disjoint(rectangle2);
			})
				->count();
			ASSERT_EQ(1, result);
	}

	TEST_F(BoundingBoxIndexTest, WithinTest)
	{
			auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::BBoxIndex>()
				->spatial("shape",
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
					std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.within(rectangle3);
			})
				->count();
			ASSERT_EQ(1, result);
	}

	TEST_F(BoundingBoxIndexTest, IntersectTest_QuadTree)
	{
		auto session = store->open_session();
			auto result = session.query<bounding_box_index_test::SpatialDoc, bounding_box_index_test::QuadTreeIndex>()
				->spatial("shape",
					[&](const documents::queries::spatial::SpatialCriteriaFactory& clause)->
					std::unique_ptr<documents::queries::spatial::SpatialCriteria>
			{
				return clause.intersect(rectangle2);
			})
				->count();
			ASSERT_EQ(0, result);
	}
}