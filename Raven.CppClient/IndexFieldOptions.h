#pragma once
#include "FieldStorage.h"
#include "FieldIndexing.h"
#include "FieldTermVector.h"
#include "SpatialOptions.h"

namespace ravendb::client::documents::indexes
{
	struct IndexFieldOptions
	{
		FieldStorage storage;
		FieldIndexing indexing;
		FieldTermVector termVector;
		SpatialOptions spatial;
		std::string analyzer;
		bool suggestions;
	};
}
