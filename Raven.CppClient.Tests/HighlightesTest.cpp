#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AbstractMultiMapIndexCreationTask.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"

namespace highlightes_test
{
	class ISearchable
	{
	public:
		virtual ~ISearchable() = 0;

		virtual const std::string& get_slug() const = 0;
		virtual void  set_slug(std::string slug) = 0;

		virtual const std::string& get_title() const = 0;
		virtual void  set_title(std::string title) = 0;

		virtual const std::string& get_content() const = 0;
		virtual void  set_content(std::string content) = 0;
	};
	ISearchable::~ISearchable() = default;

	void from_json(const nlohmann::json& j, ISearchable& is)
	{
		is.set_title(j.at("title").get<std::string>());
		is.set_slug(j.at("slug").get<std::string>());
		is.set_content(j.at("content").get<std::string>());
	}

	void to_json(nlohmann::json& j, const ISearchable& is)
	{
		j["title"] = is.get_title();
		j["slug"] = is.get_slug();
		j["content"] = is.get_content();
	}

	struct EventsItem : public ISearchable
	{
		std::string id{};
		std::string title{};
		std::string slug{};
		std::string content{};

		~EventsItem() override = default;
		const std::string& get_title() const override
		{
			return title;
		}
		void set_title(std::string title) override
		{
			this->title = std::move(title);
		}
		const std::string& get_slug() const override
		{
			return slug;
		}
		void set_slug(std::string slug) override
		{
			this->slug = std::move(slug);
		}
		const std::string& get_content() const override
		{
			return content;
		}
		void set_content(std::string content) override
		{
			this->content = std::move(content);
		}
	};

	void from_json(const nlohmann::json& j, EventsItem& ei)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		if(auto it = j.find("@metadata");
			it != j.end())
		{
			ei.id = it->at("@id").get<std::string>();
		}
		get_val_from_json(j, "title", ei.title);
		get_val_from_json(j, "slug", ei.slug);
		get_val_from_json(j, "content", ei.content);
	}

	void to_json(nlohmann::json& j, const EventsItem& ei)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "title", ei.title);
		set_val_to_json(j, "slug", ei.slug);
		set_val_to_json(j, "content", ei.content);
	}

	struct SearchResults
	{
		std::shared_ptr<ISearchable> result{};
		std::vector<std::string> highlights;
		std::string title{};
	};

	class ContentSearchIndex : public ravendb::client::documents::indexes::AbstractMultiMapIndexCreationTask
	{
	public:
		~ContentSearchIndex() override = default;
		ContentSearchIndex()
		{
			SET_DEFAULT_INDEX_NAME();

			add_map("docs.EventsItems.Select(doc => new {\n"
				"    doc = doc,\n"
				"    slug = Id(doc).ToString().Substring(Id(doc).ToString().IndexOf('/') + 1)\n"
				"}).Select(this0 => new {\n"
				"    slug = this0.slug,\n"
				"    title = this0.doc.title,\n"
				"    content = this0.doc.content\n"
				"})");

			using namespace ravendb::client::documents::indexes;

			index("slug", FieldIndexing::SEARCH);
			store("slug", FieldStorage::YES);
			term_vector("slug", FieldTermVector::WITH_POSITIONS_AND_OFFSETS);

			index("title", FieldIndexing::SEARCH);
			store("title", FieldStorage::YES);
			term_vector("title", FieldTermVector::WITH_POSITIONS_AND_OFFSETS);

			index("content", FieldIndexing::SEARCH);
			store("content", FieldStorage::YES);
			term_vector("content", FieldTermVector::WITH_POSITIONS_AND_OFFSETS);
			
		}
	};
}

namespace ravendb::client::tests::querying
{
	class HighlightesTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(highlightes_test::EventsItem, id);
		}
	};

	TEST_F(HighlightesTest, SearchWithHighlights)
	{
		std::string q = "session";

		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto events_items = std::make_shared<highlightes_test::EventsItem>();
			events_items->set_slug("ravendb-indexes-explained");
			events_items->set_title("RavenDB indexes explained");
			events_items->set_content("Itamar Syn-Hershko: Afraid of Map/Reduce? In this session,"
				" core RavenDB developer Itamar Syn-Hershko will walk through the RavenDB indexing process, grok it and much more.");
			session.store(events_items);
			session.save_changes();
		}

		highlightes_test::ContentSearchIndex().execute(store);

		{
			auto session = store->open_session();

			auto options = documents::queries::highlighting::HighlightingOptions();
			options.pre_tags.push_back("<span style='background: yellow'>");
			options.post_tags.push_back("</span>");

			std::shared_ptr<documents::queries::highlighting::Highlightings> title_highlighting{};
			std::shared_ptr<documents::queries::highlighting::Highlightings> slug_highlighting{};
			std::shared_ptr<documents::queries::highlighting::Highlightings> content_highlighting{};

			auto results = session.query<highlightes_test::ISearchable>(documents::queries::Query::index("ContentSearchIndex"))
				->wait_for_non_stale_results()
				->highlight("title", 128, 2, options, title_highlighting)
				->highlight("slug", 128, 2, options, slug_highlighting)
				->highlight("content", 128, 2, options, content_highlighting)
				->search("slug", q)->boost(15)
				->search("title", q)->boost(12)
				->search("content", q)
				->to_list([](const nlohmann::json& j)->std::shared_ptr<void>
			{
				return std::make_shared<highlightes_test::EventsItem>(j.get<highlightes_test::EventsItem>());
			});

			std::vector<highlightes_test::SearchResults> ordered_results{};
			for(const auto& searchable : results)
			{
				auto doc_id = session.advanced().get_document_id(searchable);

				auto highlights = std::vector<std::string>();

				auto title = std::string();
				auto&& titles = title_highlighting->get_fragments(*doc_id);
				if(titles.size() == 1)
				{
					title = titles[0];
				}
				else
				{
					for(const auto& t : titles)
					{
						highlights.push_back(t);
					}
				}

				for(const auto& s : slug_highlighting->get_fragments(*doc_id))
				{
					highlights.push_back(s);
				}
				for(const auto& c : content_highlighting->get_fragments(*doc_id))
				{
					highlights.push_back(c);
				}

				auto search_results = highlightes_test::SearchResults();
				search_results.result = searchable;
				search_results.highlights = std::move(highlights);
				search_results.title = title;
				ordered_results.push_back(std::move(search_results));
			}
		}
	}
}