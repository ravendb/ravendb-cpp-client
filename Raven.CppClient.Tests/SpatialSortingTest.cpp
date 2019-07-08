#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"
#include "EntityIdHelperUtil.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"

namespace ravendb::client::tests::client::spatial
{
	namespace spatial_sorting_test
	{
		struct Shop
		{
			std::string id{};
			double latitude{};
			double longitude{};

			Shop() = default;

			Shop(double latitude_param, double longitude_param)
				: latitude(latitude_param)
				, longitude(longitude_param)
			{}
		};

		void to_json(nlohmann::json& j, const Shop& s)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "latitude", s.latitude);
			set_val_to_json(j, "longitude", s.longitude);
		}

		void from_json(const nlohmann::json& j, Shop& s)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "latitude", s.latitude);
			get_val_from_json(j, "longitude", s.longitude);
		}
	}

	class SpatialSortingTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(spatial_sorting_test::Shop, id);
		}

		void create_data(std::shared_ptr<documents::IDocumentStore> store) const
		{
			auto index_definition = documents::indexes::IndexDefinition();
			index_definition.name = "eventsByLatLng";
			index_definition.maps = { "from e in docs.Shops select new { coordinates = CreateSpatialField(e.latitude, e.longitude) }" };

			auto options = documents::indexes::IndexFieldOptions();
			options.indexing = documents::indexes::FieldIndexing::EXACT;
			index_definition.fields.emplace("tag", std::move(options));

			store->maintenance()->send(documents::operations::indexes::PutIndexesOperation({ std::move(index_definition) }));

			auto index_definition2 = documents::indexes::IndexDefinition();
			index_definition2.name = "eventsByLatLngWSpecialField";
			index_definition2.maps = { "from e in docs.Shops select new { mySpacialField = CreateSpatialField(e.latitude, e.longitude) }" };

			auto options2 = documents::indexes::IndexFieldOptions();
			options2.indexing = documents::indexes::FieldIndexing::EXACT;
			index_definition2.fields.emplace("tag", std::move(options2));

			store->maintenance()->send(documents::operations::indexes::PutIndexesOperation({ std::move(index_definition2) }));

			{
				auto session = store->open_session();
				int counter{ 0 };

				for(const auto& shop : shops)
				{
					session.store(shop, "shops/" + std::to_string(++counter));
				}
				session.save_changes();
			}
			wait_for_indexing(store);
		}

		static void assert_result_order(const std::vector<std::shared_ptr<spatial_sorting_test::Shop>>& result_docs,
			const std::vector<std::string>& expected_order);

		static std::string get_query_shape_from_lat_lon(double lat, double lng, double radius);

		static constexpr double FILTERED_LAT = 44.419575;
		static constexpr double FILTERED_LNG = 34.042618;
		static constexpr double SORTED_LAT = 44.417398;
		static constexpr double SORTED_LNG = 34.042575;
		static constexpr double FILTERED_RADIUS = 100;

		static std::vector<std::shared_ptr<spatial_sorting_test::Shop>> shops;

		//shop/1:0.36KM, shop/2:0.26KM, shop/3 0.15KM from (34.042575,  44.417398)
		const static std::vector<std::string> sorted_expected_order;

		//shop/1:0.12KM, shop/2:0.03KM, shop/3 0.11KM from (34.042618,  44.419575)
		const static std::vector<std::string> filtered_expected_order;
	};

	std::vector<std::shared_ptr<spatial_sorting_test::Shop>> SpatialSortingTest::shops{
		std::make_shared<spatial_sorting_test::Shop>(44.420678, 34.042490),
		std::make_shared<spatial_sorting_test::Shop>(44.419712, 34.042232),
		std::make_shared<spatial_sorting_test::Shop>(44.418686, 34.043219) };

	const std::vector<std::string> SpatialSortingTest::sorted_expected_order{ "shops/3", "shops/2", "shops/1" };
	const std::vector<std::string> SpatialSortingTest::filtered_expected_order{ "shops/2", "shops/3", "shops/1" };

	void SpatialSortingTest::assert_result_order(const std::vector<std::shared_ptr<spatial_sorting_test::Shop>>& result_docs,
		const std::vector<std::string>& expected_order)
	{
		auto result_ids = std::vector<std::string>();
		std::transform(result_docs.cbegin(), result_docs.cend(), std::back_inserter(result_ids),
			[](const auto& doc) {return doc->id; });
		ASSERT_EQ(expected_order, result_ids);
	}

	std::string SpatialSortingTest::get_query_shape_from_lat_lon(double lat, double lng, double radius)
	{
		std::ostringstream query_shape{};
		query_shape << "Circle(" << lng << " " << lat << " d=" << radius << ")";
		return query_shape.str();
	}

	TEST_F(SpatialSortingTest, CanFilterByLocationAndSortByDistanceFromDifferentPointWDocQuery)
	{
		auto store = get_document_store(TEST_NAME);
		create_data(store);

		auto session = store->open_session();

		auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLng"))
			->spatial("coordinates",
				[](const auto& f) {return f.within(get_query_shape_from_lat_lon(FILTERED_LAT, FILTERED_LNG, FILTERED_RADIUS)); })
			->order_by_distance("coordinates", SORTED_LAT, SORTED_LNG)
			->to_list();

		assert_result_order(shops, sorted_expected_order);
	}

	TEST_F(SpatialSortingTest, CanSortByDistanceWOFilteringWDocQuery)
	{
		auto store = get_document_store(TEST_NAME);
		create_data(store);

		auto session = store->open_session();

		auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLng"))
			->order_by_distance("coordinates", SORTED_LAT, SORTED_LNG)
			->to_list();

		assert_result_order(shops, sorted_expected_order);
	}

	TEST_F(SpatialSortingTest, CanSortByDistanceWOFilteringWDocQueryBySpecifiedField)
	{
		auto store = get_document_store(TEST_NAME);
		create_data(store);

		auto session = store->open_session();

		auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLngWSpecialField"))
			->order_by_distance("mySpacialField", SORTED_LAT, SORTED_LNG)
			->to_list();

		assert_result_order(shops, sorted_expected_order);
	}

	TEST_F(SpatialSortingTest, CanSortByDistanceWOFiltering)
	{
		auto store = get_document_store(TEST_NAME);
		create_data(store);
		{
			auto session = store->open_session();

			auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLng"))
				->order_by_distance("coordinates", FILTERED_LAT, FILTERED_LNG)
				->to_list();

			assert_result_order(shops, filtered_expected_order);
		}
		{
			auto session = store->open_session();

			auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLng"))
				->order_by_distance_descending("coordinates", FILTERED_LAT, FILTERED_LNG)
				->to_list();

			std::reverse(shops.begin(), shops.end());
			assert_result_order(shops, filtered_expected_order);
		}
	}

	TEST_F(SpatialSortingTest, CanSortByDistanceWOFilteringBySpecifiedField)
	{
		auto store = get_document_store(TEST_NAME);
		create_data(store);
		{
			auto session = store->open_session();

			auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLngWSpecialField"))
				->order_by_distance("mySpacialField", FILTERED_LAT, FILTERED_LNG)
				->to_list();

			assert_result_order(shops, filtered_expected_order);
		}
		{
			auto session = store->open_session();

			auto shops = session.query<spatial_sorting_test::Shop>(documents::queries::Query::index("eventsByLatLngWSpecialField"))
				->order_by_distance_descending("mySpacialField", FILTERED_LAT, FILTERED_LNG)
				->to_list();

			std::reverse(shops.begin(), shops.end());
			assert_result_order(shops, filtered_expected_order);
		}
	}
}
