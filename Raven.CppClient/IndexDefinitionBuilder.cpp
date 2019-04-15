#include "stdafx.h"
#include "IndexDefinitionBuilder.h"
#include "GetCppClassName.h"

namespace ravendb::client::documents::indexes
{
	IndexDefinitionBuilder::IndexDefinitionBuilder(std::optional<std::string> index_name)
		: _index_name([&]()
	{
		auto temp_index_name = index_name ? *std::move(index_name) : impl::utils::GetCppClassName::get_simple_class_name(typeid(*this));
		if(temp_index_name.length() > 256)
		{
			throw std::invalid_argument("The index name is limited to 256 characters, but was: " + temp_index_name);
		}
		return std::move(temp_index_name);
	}())
	{}

	IndexDefinition IndexDefinitionBuilder::to_index_definition(
		std::shared_ptr<conventions::DocumentConventions> conventions, bool validate_map) const
	{
		if(!map && validate_map)
		{
			std::ostringstream msg{};
			msg << "Map is required to generate an index," <<
				" you cannot create an index without a valid map property (in index " <<
				_index_name << ").";
			throw std::runtime_error(msg.str());
		}

		try
		{
			auto index_definition = IndexDefinition();
			index_definition.name = _index_name;
			index_definition.reduce = reduce;
			index_definition.lock_mode = lock_mode;
			index_definition.priority = priority;
			index_definition.output_reduce_to_collection = output_reduce_to_collection;

			auto suggestions = std::unordered_map<std::string, bool>();
			for(const auto& suggestions_option : suggestion_options)
			{
				suggestions.emplace(std::pair(suggestions_option, true));
			}

			apply_values<FieldIndexing>(index_definition, indexes_strings, 
				[](auto& options, const auto& value)
			{
				options.indexing = value;
			});
			apply_values<FieldStorage>(index_definition, stores_strings,
				[](auto& options, const auto& value)
			{
				options.storage = value;
			});
			apply_values<std::string>(index_definition, analyzers_strings,
				[](auto& options, const auto& value)
			{
				options.analyzer = value;
			});
			apply_values<FieldTermVector>(index_definition, term_vector_strings,
				[](auto& options, const auto& value)
			{
				options.term_vector = value;
			});
			apply_values<spatial::SpatialOptions>(index_definition, spatial_indexes_strings,
				[](auto& options, const auto& value)
			{
				options.spatial = value;
			});
			apply_values<bool>(index_definition, suggestions,
				[](auto& options, const auto& value)
			{
				options.suggestions = value;
			});

			if(map)
			{
				index_definition.maps.insert(*map);
			}
			index_definition.additional_sources = additional_sources;

			return std::move(index_definition);
		}
		catch (std::exception& e)
		{
			//TODO throw new IndexCompilationException("Failed to create index " + _indexName, e);
			throw;
		}
	}
}
