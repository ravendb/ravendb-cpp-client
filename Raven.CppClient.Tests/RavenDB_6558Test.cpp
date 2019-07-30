#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AbstractMultiMapIndexCreationTask.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"

namespace RavenDB_6558Test_ns
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

		if (auto it = j.find("@metadata");
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

namespace ravendb::client::tests::issues
{
	class RavenDB_6558Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(RavenDB_6558Test_ns::EventsItem, id);
		}
	};

	TEST_F(RavenDB_6558Test, CanUseDifferentPreAndPostTagsPerField)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto events_items = std::make_shared<RavenDB_6558Test_ns::EventsItem>();
			events_items->set_slug("ravendb-indexes-explained");
			events_items->set_title("RavenDB indexes explained");
			events_items->set_content("Itamar Syn-Hershko: Afraid of Map/Reduce? In this session,"
				" core RavenDB developer Itamar Syn-Hershko will walk through the RavenDB indexing process, grok it and much more.");
			session.store(events_items, "items/1");
			session.save_changes();
		}

		RavenDB_6558Test_ns::ContentSearchIndex().execute(store);

		auto options1 = documents::queries::highlighting::HighlightingOptions();
		options1.pre_tags.push_back("***");
		options1.post_tags.push_back("***");

		auto options2 = documents::queries::highlighting::HighlightingOptions();
		options2.pre_tags.push_back("^^^");
		options2.post_tags.push_back("^^^");

		auto session = store->open_session();

		std::shared_ptr<documents::queries::highlighting::Highlightings> title_highlighting{};
		std::shared_ptr<documents::queries::highlighting::Highlightings> content_highlighting{};

		auto results = session.query<RavenDB_6558Test_ns::ISearchable>(documents::queries::Query::index("ContentSearchIndex"))
			->wait_for_non_stale_results()
			->highlight("title", 128, 2, options1, title_highlighting)
			->highlight("content", 128, 2, options2, content_highlighting)
			->search("title", "RavenDB")->boost(12)
			->search("content", "RavenDB")
			->to_list([](const nlohmann::json& j)->std::shared_ptr<void>
		{
			return std::make_shared<RavenDB_6558Test_ns::EventsItem>(j.get<RavenDB_6558Test_ns::EventsItem>());
		});

		ASSERT_TRUE(title_highlighting->get_fragments("items/1").at(0).find("***") != std::string::npos);
		ASSERT_TRUE(content_highlighting->get_fragments("items/1").at(0).find("^^^") != std::string::npos);
	}
}