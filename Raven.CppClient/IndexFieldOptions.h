#pragma once
#include "FieldStorage.h"
#include "FieldIndexing.h"
#include "FieldTermVector.h"
#include "SpatialOptions.h"
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::indexes
{
	struct IndexFieldOptions
	{
		FieldStorage storage = FieldStorage::UNSET;
		FieldIndexing indexing = FieldIndexing::UNSET;
		FieldTermVector term_vector = FieldTermVector::UNSET;
		std::optional<spatial::SpatialOptions> spatial{};
		std::string analyzer{};
		std::optional<bool> suggestions{};


		friend bool operator==(const IndexFieldOptions& lhs, const IndexFieldOptions& rhs);
	};

	void to_json(nlohmann::json& j, const IndexFieldOptions& ifo);

	void from_json(const nlohmann::json& j, IndexFieldOptions& ifo);
}
