#pragma once
#include "FieldStorage.h"
#include "FieldIndexing.h"
#include "FieldTermVector.h"
#include "SpatialOptions.h"
#include "IndexLockMode.h"
#include "IndexPriority.h"
#include "IndexDefinition.h"
#include "DocumentConventions.h"

namespace ravendb::client::documents::indexes
{
	class IndexDefinitionBuilder
	{
	private:
		const std::string _index_name;

		template<typename T>
		void apply_values(IndexDefinition& index_definition,
			const std::unordered_map<std::string, T>& values,
			std::function<void(IndexFieldOptions&, const T&)> action) const;

	public:
		std::optional<std::string> map{};
		std::optional<std::string> reduce{};

		std::unordered_map<std::string, FieldStorage> stores_strings{};
		std::unordered_map<std::string, FieldIndexing> indexes_strings{};
		std::unordered_map<std::string, std::string> analyzers_strings{};
		std::unordered_set<std::string> suggestion_options{};
		std::unordered_map<std::string, FieldTermVector> term_vector_strings{};
		std::unordered_map<std::string, spatial::SpatialOptions> spatial_indexes_strings{};
		std::optional<IndexLockMode> lock_mode{};
		std::optional<IndexPriority> priority{};
		std::optional<std::string> output_reduce_to_collection{};
		std::optional<std::unordered_map<std::string, std::string>> additional_sources{};

		IndexDefinitionBuilder(std::optional<std::string> index_name = {});

		IndexDefinition to_index_definition(
			std::shared_ptr<conventions::DocumentConventions> conventions,
			bool validate_map = true) const;
	};

	template <typename T>
	void IndexDefinitionBuilder::apply_values(IndexDefinition& index_definition,
		const std::unordered_map<std::string, T>& values, std::function<void(IndexFieldOptions&, const T&)> action) const
	{
		for(const auto& [key, val] : values)
		{
			action(index_definition.fields[key], val);
		}
	}
}
