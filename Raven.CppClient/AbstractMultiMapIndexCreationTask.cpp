#include "stdafx.h"
#include "AbstractMultiMapIndexCreationTask.h"
#include "IndexDefinitionBuilder.h"

namespace ravendb::client::documents::indexes
{
	void AbstractMultiMapIndexCreationTask::add_map(std::string map)
	{
		_maps.push_back(std::move(map));
	}

	AbstractMultiMapIndexCreationTask::~AbstractMultiMapIndexCreationTask() = default;

	IndexDefinition AbstractMultiMapIndexCreationTask::create_index_definition()
	{
		if (!conventions)
		{
			conventions = conventions::DocumentConventions::create();
		}

		auto index_definition_builder = IndexDefinitionBuilder(get_index_name());
		index_definition_builder.indexes_strings = indexes_strings;
		index_definition_builder.analyzers_strings = analyzers_strings;
		index_definition_builder.map = map;
		index_definition_builder.reduce = reduce;
		index_definition_builder.stores_strings = stores_strings;
		index_definition_builder.suggestion_options = index_suggestions;
		index_definition_builder.term_vector_strings = term_vectors_strings;
		index_definition_builder.spatial_indexes_strings = spatial_options_strings;
		index_definition_builder.output_reduce_to_collection = output_reduce_to_collections;
		index_definition_builder.additional_sources = additional_sources;

		auto&& index_definition = index_definition_builder.to_index_definition(conventions, false);
		index_definition.maps = std::unordered_set<std::string>(_maps.cbegin(), _maps.cend());

		return index_definition;
	}
}
