#include "stdafx.h"
#include "Query.h"

namespace ravendb::client::documents::queries
{
	const std::optional<std::string>& Query::get_collection() const
	{
		return _collection;
	}

	const std::optional<std::string>& Query::get_index_name() const
	{
		return _index_name;
	}

	Query Query::index(std::string index_name)
	{
		auto query = Query();
		query._index_name = std::move(index_name);
		return query;
	}

	Query Query::collection(std::string collection_name)
	{
		auto query = Query();
		query._collection = std::move(collection_name);
		return query;
	}
}
