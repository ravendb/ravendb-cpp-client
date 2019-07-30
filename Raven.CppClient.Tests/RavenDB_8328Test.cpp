#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "PointField.h"
#include "WktField.h"

namespace RavenDB_8328Test_ns
{
	struct Item
	{
		std::string id{};
		std::string name{};
		double latitude{};
		double longitude{};
		double latitude2{};
		double longitude2{};
		std::string shapeWkt{};
	};

	void from_json(const nlohmann::json& j, Item& i)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", i.name);
		get_val_from_json(j, "latitude", i.latitude);
		get_val_from_json(j, "longitude", i.longitude);
		get_val_from_json(j, "latitude2", i.latitude2);
		get_val_from_json(j, "longitude2", i.longitude2);
		get_val_from_json(j, "shapeWkt", i.shapeWkt);
	}

	void to_json(nlohmann::json& j, const Item& i)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", i.name);
		set_val_to_json(j, "latitude", i.latitude);
		set_val_to_json(j, "longitude", i.longitude);
		set_val_to_json(j, "latitude2", i.latitude2);
		set_val_to_json(j, "longitude2", i.longitude2);
		set_val_to_json(j, "shapeWkt", i.shapeWkt);
	}
}

namespace ravendb::client::tests::issues
{
	class RavenDB_8328Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(RavenDB_8328Test_ns::Item, id);
		}
	};

	TEST_F(RavenDB_8328Test, SpatialOnAutoIndex)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto item = std::make_shared<RavenDB_8328Test_ns::Item>();
			item->latitude = 10;
			item->longitude = 20;
			item->latitude2 = 10;
			item->longitude2 = 20;
			item->shapeWkt = "POINT(20 10)";
			item->name = "Name1";

			session.store(item);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto q = session.query<RavenDB_8328Test_ns::Item>()
				->spatial(documents::queries::spatial::PointField("latitude", "longitude"),
					[](const auto& f) {return f.within_radius(10, 10, 20); });

			auto iq = q->get_index_query();
			ASSERT_EQ("from Items where spatial.within(spatial.point(latitude, longitude), spatial.circle($p0, $p1, $p2))", iq.query);

			q = session.query<RavenDB_8328Test_ns::Item>()
				->spatial(documents::queries::spatial::WktField("shapeWkt"),
					[](const auto& f) { return f.within_radius(10, 10, 20); });

			iq = q->get_index_query();
			ASSERT_EQ("from Items where spatial.within(spatial.wkt(shapeWkt), spatial.circle($p0, $p1, $p2))", iq.query);
		}
		{
			auto session = store->open_session();

			auto stats_ref = std::shared_ptr<documents::session::QueryStatistics>{};
			auto results = session.query<RavenDB_8328Test_ns::Item>()
				->statistics(stats_ref)
				->spatial(documents::queries::spatial::PointField("latitude", "longitude"),
					[](const auto& f) {return f.within_radius(10, 10, 20); })
				->to_list();

			ASSERT_EQ(1, results.size());

			ASSERT_EQ("Auto/Items/BySpatial.point(latitude|longitude)", stats_ref->index_name);

			stats_ref.reset();

			results = session.query<RavenDB_8328Test_ns::Item>()
				->statistics(stats_ref)
				->spatial(documents::queries::spatial::PointField("latitude2", "longitude2"),
					[](const auto& f) {return f.within_radius(10, 10, 20); })
				->to_list();

			ASSERT_EQ(1, results.size());
			ASSERT_EQ("Auto/Items/BySpatial.point(latitude|longitude)AndSpatial.point(latitude2|longitude2)", stats_ref->index_name);

			stats_ref.reset();
			results = session.query<RavenDB_8328Test_ns::Item>()
				->statistics(stats_ref)
				->spatial(documents::queries::spatial::WktField("shapeWkt"),
					[](const auto& f) {return f.within_radius(10, 10, 20); })
				->to_list();

			ASSERT_EQ(1, results.size());
			ASSERT_EQ("Auto/Items/BySpatial.point(latitude|longitude)AndSpatial.point(latitude2|longitude2)AndSpatial.wkt(shapeWkt)",
				stats_ref->index_name);
		}
	}
}