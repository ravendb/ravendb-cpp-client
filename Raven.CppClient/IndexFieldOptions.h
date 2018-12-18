#pragma once
#include "FieldStorage.h"
#include "FieldIndexing.h"
#include "FieldTermVector.h"
#include "SpatialOptions.h"

namespace ravendb::client::documents::indexes
{
	struct IndexFieldOptions
	{
		FieldStorage storage = FieldStorage::UNSET;
		FieldIndexing indexing = FieldIndexing::UNSET;
		FieldTermVector term_vector = FieldTermVector::UNSET;
		std::optional<spacial::SpatialOptions> spatial{};
		std::string analyzer{};
		std::optional<bool> suggestions{};


		friend bool operator==(const IndexFieldOptions& lhs, const IndexFieldOptions& rhs)
		{
			return lhs.storage == rhs.storage
				&& lhs.indexing == rhs.indexing
				&& lhs.term_vector == rhs.term_vector
				&& lhs.spatial == rhs.spatial
				&& lhs.analyzer == rhs.analyzer
				&& lhs.suggestions == rhs.suggestions;
		}
	};

	inline void to_json(nlohmann::json& j, const IndexFieldOptions& ifo)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Storage", ifo.storage);
		set_val_to_json(j, "Indexing", ifo.indexing);
		set_val_to_json(j, "TermVector", ifo.term_vector);
		set_val_to_json(j, "Spacial", ifo.spatial);
		set_val_to_json(j, "Analyzer", ifo.analyzer);
		set_val_to_json(j, "Suggestions", ifo.suggestions);
	}

	inline void from_json(const nlohmann::json& j, IndexFieldOptions& ifo)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Storage", ifo.storage);
		get_val_from_json(j, "Indexing", ifo.indexing);
		get_val_from_json(j, "TermVector", ifo.term_vector);
		get_val_from_json(j, "Spacial", ifo.spatial);
		get_val_from_json(j, "Analyzer", ifo.analyzer);
		get_val_from_json(j, "Suggestions", ifo.suggestions);
	}
}
