#pragma once
#include "Highlightings.h"


namespace ravendb::client::documents::queries
{
	struct QueryResult;
}

namespace ravendb::client::documents::queries::highlighting
{
	struct QueryHighlightings
	{
	private:
		std::vector<Highlightings> _highlightings{};

	public:
		Highlightings add(std::string field_name);

		void update(const QueryResult& query_result);
	};
}
