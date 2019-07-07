#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"
#include "EntityIdHelperUtil.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"
#include "Query.h"

namespace ravendb::client::tests::client::spatial
{
	namespace spatial_queries_test
	{
		struct DummyGeoDoc
		{
			std::string id{};
			double latitude{};
			double longitude{};

			DummyGeoDoc(double latitude_param, double longitude_param)
				: latitude(latitude_param)
				, longitude(longitude_param)
			{}
		};

		void to_json(nlohmann::json& j, const DummyGeoDoc& dgd)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "latitude", dgd.latitude);
			set_val_to_json(j, "longitude", dgd.longitude);
		}

		void from_json(const nlohmann::json& j, DummyGeoDoc& dgd)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "latitude", dgd.latitude);
			get_val_from_json(j, "longitude", dgd.longitude);
		}
	}

	class SpatialQueriesTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			//REGISTER_ID_PROPERTY_FOR(spatial_queries_test::DummyGeoDoc, id);
		}

		class SpatialQueriesInMemoryTestIdx : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~SpatialQueriesInMemoryTestIdx() override = default;
			SpatialQueriesInMemoryTestIdx()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "docs.Listings.Select(listingItem => new {\n"
					"    classCodes = listingItem.classCodes,\n"
					"    latitude = listingItem.latitude,\n"
					"    longitude = listingItem.longitude,\n"
					"    coordinates = this.CreateSpatialField(((double ? )((double)(listingItem.latitude))), ((double ? )((double)(listingItem.longitude))))\n"
					"})";
			}
		};
	};

	TEST_F(SpatialQueriesTest, CanSuccessfullyDoSpatialQueryOfNearbyLocations)
	{
		// These items is in a radius of 4 miles (approx 6,5 km)
		auto area_one_doc_one = std::make_shared<spatial_queries_test::DummyGeoDoc>(55.6880508001, 13.5717346673);
		auto area_one_doc_two = std::make_shared<spatial_queries_test::DummyGeoDoc>(55.6821978456, 13.6076183965);
		auto area_one_doc_three = std::make_shared<spatial_queries_test::DummyGeoDoc>(55.673251569, 13.5946697607);

		// This item is 12 miles (approx 19 km) from the closest in area_one
		auto close_but_outside_area_one = std::make_shared<spatial_queries_test::DummyGeoDoc>(55.8634157297, 13.5497731987);

		// This item is about 3900 miles from area_one
		auto new_york = std::make_shared<spatial_queries_test::DummyGeoDoc>(40.7137578228, -74.0126901936);

		auto store = get_document_store(TEST_NAME);
		auto session = store->open_session();

		session.store(area_one_doc_one);
		session.store(area_one_doc_two);
		session.store(area_one_doc_three);
		session.store(close_but_outside_area_one);
		session.store(new_york);
		session.save_changes();

		auto index_definition = documents::indexes::IndexDefinition();
		index_definition.name = "FindByLatLng";
		index_definition.maps = { "from doc in docs select new { coordinates = CreateSpatialField(doc.latitude, doc.longitude) }" };

		store->maintenance()->send(documents::operations::indexes::PutIndexesOperation({ index_definition }));

		// Wait until the index is built
		session.query<spatial_queries_test::DummyGeoDoc>(documents::queries::Query::index("FindByLatLng"))
			->wait_for_non_stale_results()
			->to_list();

		constexpr double lat = 55.6836422426, lng = 13.5871808352; // in the middle of AreaOne
		constexpr double radius = 5.0;

		auto nearby_docs = session.query<spatial_queries_test::DummyGeoDoc>(documents::queries::Query::index("FindByLatLng"))
			->within_radius_of("coordinates", radius, lat, lng)
			->wait_for_non_stale_results()
			->to_list();

		ASSERT_EQ(3, nearby_docs.size());
	}

	TEST_F(SpatialQueriesTest, CanSuccessfullyQueryByMiles)
	{
		auto my_house = std::make_shared<spatial_queries_test::DummyGeoDoc>(44.757767, -93.355322);

		// The gym is about 7.32 miles (11.79 kilometers) from my house.
		auto gym = std::make_shared<spatial_queries_test::DummyGeoDoc>(44.682861, -93.25);

		auto store = get_document_store(TEST_NAME);
		auto session = store->open_session();

		session.store(my_house);
		session.store(gym);
		session.save_changes();

		auto index_definition = documents::indexes::IndexDefinition();
		index_definition.name = "FindByLatLng";
		index_definition.maps = { "from doc in docs select new { coordinates = CreateSpatialField(doc.latitude, doc.longitude) }" };
		store->maintenance()->send(documents::operations::indexes::PutIndexesOperation({ index_definition }));

		// Wait until the index is built
		session.query<spatial_queries_test::DummyGeoDoc>(documents::queries::Query::index("FindByLatLng"))
			->wait_for_non_stale_results()
			->to_list();

		constexpr double radius = 8;

		// Find within 8 miles.
		// We should find both my house and the gym.
		auto mathches_within_miles = session.query<spatial_queries_test::DummyGeoDoc>(documents::queries::Query::index("FindByLatLng"))
			->within_radius_of("coordinates", radius, my_house->latitude, my_house->longitude, documents::indexes::spatial::SpatialUnits::MILES)
			->wait_for_non_stale_results()
			->to_list();

		ASSERT_EQ(2, mathches_within_miles.size());

		// Find within 8 kilometers.
		// We should find only my house, since the gym is ~11 kilometers out.

		auto mathches_within_kilometers = session.query<spatial_queries_test::DummyGeoDoc>(documents::queries::Query::index("FindByLatLng"))
			->within_radius_of("coordinates", radius, my_house->latitude, my_house->longitude, documents::indexes::spatial::SpatialUnits::KILOMETERS)
			->wait_for_non_stale_results()
			->to_list();

		ASSERT_EQ(1, mathches_within_kilometers.size());
	}
}
