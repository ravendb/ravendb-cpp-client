#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::tests::client::spatial
{
	namespace simon_bartlett_test
	{
		struct GeoDocument
		{
			std::string wkt{};
		};

		void to_json(nlohmann::json& j, const GeoDocument& gd)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "WKT", gd.wkt);
		}

		void from_json(const nlohmann::json& j, GeoDocument& gd)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "WKT", gd.wkt);
		}
	}

	class SimonBartlettTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		class GeoIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~GeoIndex() override = default;
			GeoIndex()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "docs.GeoDocuments.Select(doc => new {\n" 
					"    WKT = this.CreateSpatialField(doc.WKT)\n"
					"})";

				auto spatial_options = documents::indexes::spatial::SpatialOptions();
				spatial_options.strategy = documents::indexes::spatial::SpatialSearchStrategy::GEOHASH_PREFIX_TREE;
				spatial_options_strings.emplace("WKT", std::move(spatial_options));
			}
		};
	};

	TEST_F(SimonBartlettTest, LineStringsShouldIntersect)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<GeoIndex>());

		{
			auto session = store->open_session();
			auto geo_document = std::make_shared<simon_bartlett_test::GeoDocument>();
			geo_document->wkt = "LINESTRING (0 0, 1 1, 2 1)";
			session.store(geo_document);
			session.save_changes();
		}
		wait_for_indexing(store);
		{
			auto session = store->open_session();

			auto count = session.query<simon_bartlett_test::GeoDocument, GeoIndex>()
				->spatial("WKT", [](auto f)
			{return f.relates_to_shape("LINESTRING (1 0, 1 1, 1 2)", documents::indexes::spatial::SpatialRelation::INTERSECTS); })
				->wait_for_non_stale_results()
				->count();

			ASSERT_EQ(1, count);

			count = session.query<simon_bartlett_test::GeoDocument, GeoIndex>()
				->relates_to_shape("WKT", "LINESTRING (1 0, 1 1, 1 2)", documents::indexes::spatial::SpatialRelation::INTERSECTS)
				->wait_for_non_stale_results()
				->count();

			ASSERT_EQ(1, count);
		}
	}

	TEST_F(SimonBartlettTest, CirclesShouldNotIntersect)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<GeoIndex>());

		{
			auto session = store->open_session();
			// 110km is approximately 1 degree
			auto geo_document = std::make_shared<simon_bartlett_test::GeoDocument>();
			geo_document->wkt = "CIRCLE(0.000000 0.000000 d=110)";
			session.store(geo_document);
			session.save_changes();
		}
		wait_for_indexing(store);
		{
			auto session = store->open_session();
			// Should not intersect, as there is 1 Degree between the two shapes
			auto count = session.query<simon_bartlett_test::GeoDocument, GeoIndex>()
				->spatial("WKT", [](auto f)
			{return f.relates_to_shape("CIRCLE(0.000000 3.000000 d=110)", documents::indexes::spatial::SpatialRelation::INTERSECTS); })
				->wait_for_non_stale_results()
				->count();

			ASSERT_EQ(0, count);

			count = session.query<simon_bartlett_test::GeoDocument, GeoIndex>()
				->relates_to_shape("WKT", "CIRCLE(0.000000 3.000000 d=110)", documents::indexes::spatial::SpatialRelation::INTERSECTS)
				->wait_for_non_stale_results()
				->count();

			ASSERT_EQ(0, count);
		}
	}
}
