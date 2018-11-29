#pragma once
#include "FieldStorage.h"
#include "FieldIndexing.h"
#include "FieldTermVector.h"
#include "SpatialOptions.h"

namespace ravendb::client::documents::indexes
{
	struct IndexFieldOptions
	{
		FieldStorage storage;//what default should be ?
		FieldIndexing indexing;//what default should be ?
		FieldTermVector termVector//what default should be ?;
		SpatialOptions spatial{};
		std::string analyzer{};
		bool suggestions;//what default should be ?
	};
}
