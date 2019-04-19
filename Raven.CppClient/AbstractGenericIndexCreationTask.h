#pragma once
#include "AbstractIndexCreationTaskBase.h"

namespace ravendb::client::documents::indexes
{
	namespace spatial
	{
		class SpatialOptionsFactory;
	}

	class AbstractGenericIndexCreationTask : public AbstractIndexCreationTaskBase
	{
	protected:
		std::string map{};
		std::optional<std::string> reduce{};

		std::unordered_map<std::string, FieldStorage> stores_strings{};
		std::unordered_map<std::string, FieldIndexing> indexes_strings{};
		std::unordered_map<std::string, std::string> analyzers_strings{};
		std::unordered_set<std::string> index_suggestions{};
		std::unordered_map<std::string, FieldTermVector> term_vectors_strings{};
		std::unordered_map<std::string, spatial::SpatialOptions> spacial_options_strings{};

		std::string output_reduce_to_collections{};

	public:
		~AbstractGenericIndexCreationTask() override = 0;

		bool is_map_reduce() const override;

	protected:
		void index(std::string field, FieldIndexing indexing);

		void spacial(std::string field, std::function<spatial::SpatialOptions(const spatial::SpatialOptionsFactory&)> indexing);

		void store_all_fields(FieldStorage storage);

		void store(std::string field, FieldStorage storage);

		void analyze(std::string field, std::string analyzer);

		void term_vector(std::string field, FieldTermVector term_vector);

		void suggestion(std::string field);
	};
}
