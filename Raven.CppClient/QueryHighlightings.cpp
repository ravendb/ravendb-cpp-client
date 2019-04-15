#include "stdafx.h"
#include "QueryHighlightings.h"
#include "QueryResult.h"

namespace ravendb::client::documents::queries::highlighting
{
	Highlightings QueryHighlightings::add(std::string field_name)
	{
		auto field_highlightings = Highlightings(std::move(field_name));
		_highlightings.push_back(field_highlightings);
		return std::move(field_highlightings);
	}

	void QueryHighlightings::update(const QueryResult& query_result)
	{
		for(auto& field_highlightings : _highlightings)
		{
			field_highlightings.update(query_result.highlightings);
		}
	}
}
