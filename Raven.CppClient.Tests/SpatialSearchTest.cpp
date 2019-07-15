#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::tests::client::spatial
{
	namespace spatial_search_test
	{
		struct Event
		{
			std::string venue{};
			double latitude{};
			double longitude{};
			std::optional<impl::DateTimeOffset> date{};
			int32_t capacity{};

			Event() = default;

			Event(std::string venue_param,
				double latitude_param, double longitude_param,
				std::optional<impl::DateTimeOffset> date_param = {},
				int32_t capacity = {})
				: venue(std::move(venue_param))
				, latitude(latitude_param)
				, longitude(longitude_param)
				, date(date_param)
				, capacity(capacity)
			{}
		};

		void to_json(nlohmann::json& j, const Event& e)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "venue", e.venue);
			set_val_to_json(j, "latitude", e.latitude);
			set_val_to_json(j, "longitude", e.longitude);
			set_val_to_json(j, "date", e.date);
			set_val_to_json(j, "capacity", e.capacity);

		}

		void from_json(const nlohmann::json& j, Event& e)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "venue", e.venue);
			get_val_from_json(j, "latitude", e.latitude);
			get_val_from_json(j, "longitude", e.longitude);
			get_val_from_json(j, "date", e.date);
			get_val_from_json(j, "capacity", e.capacity);
		}
	}

	class SpatialSearchTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		class SpatialIdx : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~SpatialIdx() override = default;
			SpatialIdx()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "docs.Events.Select(e => new {\n"
					"    capacity = e.capacity,\n"
					"    venue = e.venue,\n"
					"    date = e.date,\n"
					"    coordinates = this.CreateSpatialField(((double ? ) e.latitude), ((double ? ) e.longitude))\n"
					"})";
				index("venue", documents::indexes::FieldIndexing::SEARCH);
			}
		};
	};

	TEST_F(SpatialSearchTest, CanDoSpatialSearchWithClientApi)
	{
		auto store = get_document_store(TEST_NAME);
		SpatialIdx().execute(store);

		{
			auto session = store->open_session();
			session.store(std::make_shared<spatial_search_test::Event>("a/1", 38.9579000, -77.3572000));
			session.store(std::make_shared<spatial_search_test::Event>("a/2", 38.9690000, -77.3862000,
				impl::DateTimeOffset("2000-01-02T00:00:00.0000000")));
			session.store(std::make_shared<spatial_search_test::Event>("b/2", 38.9690000, -77.3862000,
			    impl::DateTimeOffset("2000-01-03T00:00:00.0000000")));
			session.store(std::make_shared<spatial_search_test::Event>("c/3", 38.9510000, -77.4107000,
			    impl::DateTimeOffset("2003-01-01T00:00:00.0000000")));
			session.store(std::make_shared<spatial_search_test::Event>("d/1", 37.9510000, -77.4107000,
			    impl::DateTimeOffset("2003-01-01T00:00:00.0000000")));
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();

			auto stats_ref = std::shared_ptr<documents::session::QueryStatistics>();

			auto events = session.query<spatial_search_test::Event>(documents::queries::Query::index("SpatialIdx"))
				->statistics(stats_ref)
				->where_less_than_or_equal("date", impl::DateTimeOffset("2001-01-01T00:00:00.0000000"))
				->within_radius_of("coordinates", 6.0, 38.96939, -77.386398)
				->order_by_descending("date")
				->to_list();

			ASSERT_FALSE(events.empty());
		}
	}

	TEST_F(SpatialSearchTest, CanDoSpatialSearchWithClientApi3)
	{
		auto store = get_document_store(TEST_NAME);
		SpatialIdx().execute(store);

		auto session = store->open_session();

		auto matching_venues = session.advanced().document_query<spatial_search_test::Event, SpatialIdx>()
			->spatial("coordinates", [](const auto& f) {return f.within_radius(5, 38.9103000, -77.3942); })
			->wait_for_non_stale_results();

		auto iq = matching_venues->get_index_query();

		ASSERT_EQ("from index 'SpatialIdx' where spatial.within(coordinates, spatial.circle($p0, $p1, $p2))", iq.query);
		ASSERT_EQ(5.0, iq.query_parameters.at("p0").get<double>());
		ASSERT_EQ(38.9103, iq.query_parameters.at("p1").get<double>());
		ASSERT_EQ(-77.3942, iq.query_parameters.at("p2").get<double>());
	}

	TEST_F(SpatialSearchTest, CanDoSpatialSearchWithClientApiWithinGivenCapacity)
	{
		auto store = get_document_store(TEST_NAME);
		SpatialIdx().execute(store);

		{
			auto session = store->open_session();
			session.store(std::make_shared<spatial_search_test::Event>("a/1", 38.9579000, -77.3572000, 
				std::optional<impl::DateTimeOffset>(), 5000));
			session.store(std::make_shared<spatial_search_test::Event>("a/2", 38.9690000, -77.3862000,
			    impl::DateTimeOffset("2000-01-02T00:00:00.0000000"), 5000));
			session.store(std::make_shared<spatial_search_test::Event>("b/2", 38.9690000, -77.3862000,
			    impl::DateTimeOffset("2000-01-03T00:00:00.0000000"), 2000));
			session.store(std::make_shared<spatial_search_test::Event>("c/3", 38.9510000, -77.4107000,
			    impl::DateTimeOffset("2003-01-01T00:00:00.0000000"), 1500));
			session.store(std::make_shared<spatial_search_test::Event>("d/1", 37.9510000, -77.4107000,
			    impl::DateTimeOffset("2003-01-01T00:00:00.0000000"), 1500));
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();
			auto query_stats = std::shared_ptr<documents::session::QueryStatistics>();

			auto events = session.query<spatial_search_test::Event>(documents::queries::Query::index("SpatialIdx"))
				->statistics(query_stats)
				->open_subclause()
					->where_greater_than_or_equal("capacity", 0)
					->and_also()
					->where_less_than_or_equal("capacity", 2000)
				->close_subclause()
				->within_radius_of("coordinates", 6.0, 38.96939, -77.386398)
				->order_by_descending("date")
				->to_list();

			ASSERT_EQ(2, query_stats->total_results);
			std::vector<std::string> venues{};
			std::transform(events.cbegin(), events.cend(), std::back_inserter(venues),
				[](const auto& event)
			{
				return event->venue;
			});
			auto venues_to_compare = std::vector<std::string>{ "c/3", "b/2" };
			ASSERT_EQ(venues_to_compare, venues);
		}
	}

	TEST_F(SpatialSearchTest, CanDoSpatialSearchWithClientApiAddOrder)
	{
		auto store = get_document_store(TEST_NAME);
		SpatialIdx().execute(store);

		{
			auto session = store->open_session();

			session.store(std::make_shared<spatial_search_test::Event>("a/1", 38.9579000, -77.3572000));
			session.store(std::make_shared<spatial_search_test::Event>("b/1", 38.9579000, -77.3572000));
			session.store(std::make_shared<spatial_search_test::Event>("c/1", 38.9579000, -77.3572000));
			session.store(std::make_shared<spatial_search_test::Event>("a/2", 38.9690000, -77.3862000));
			session.store(std::make_shared<spatial_search_test::Event>("b/2", 38.9690000, -77.3862000));
			session.store(std::make_shared<spatial_search_test::Event>("c/2", 38.9690000, -77.3862000));
			session.store(std::make_shared<spatial_search_test::Event>("a/3", 38.9510000, -77.4107000));
			session.store(std::make_shared<spatial_search_test::Event>("b/3", 38.9510000, -77.4107000));
			session.store(std::make_shared<spatial_search_test::Event>("c/3", 38.9510000, -77.4107000));
			session.store(std::make_shared<spatial_search_test::Event>("d/1", 37.9510000, -77.4107000));
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();
			auto events = session.query<spatial_search_test::Event>(documents::queries::Query::index("spatialIdx"))
				->within_radius_of("coordinates", 6.0, 38.96939, -77.386398)
				->order_by_distance("coordinates", 38.96939, -77.386398)
				->add_order("venue", false)
				->to_list();

			std::vector<std::string> venues{};
			std::transform(events.cbegin(), events.cend(), std::back_inserter(venues),
				[](const auto& event)
			{
				return event->venue;
			});
			auto venues_to_compare = std::vector<std::string>{ "a/2", "b/2", "c/2", "a/1", "b/1", "c/1", "a/3", "b/3", "c/3" };
			ASSERT_EQ(venues_to_compare, venues);
		}
		{
			auto session = store->open_session();
			auto events = session.query<spatial_search_test::Event>(documents::queries::Query::index("spatialIdx"))
				->within_radius_of("coordinates", 6.0, 38.96939, -77.386398)
				->add_order("venue", false)
				->order_by_distance("coordinates", 38.96939, -77.386398)
				->to_list();

			std::vector<std::string> venues{};
			std::transform(events.cbegin(), events.cend(), std::back_inserter(venues),
				[](const auto& event)
			{
				return event->venue;
			});
			auto venues_to_compare = std::vector<std::string>{ "a/1", "a/2", "a/3", "b/1", "b/2", "b/3", "c/1", "c/2", "c/3" };
			ASSERT_EQ(venues_to_compare, venues);
		}
	}
}
