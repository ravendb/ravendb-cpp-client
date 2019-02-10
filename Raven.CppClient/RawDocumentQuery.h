#pragma once
#include "AbstractDocumentQuery.h"

namespace ravendb::client::documents::session
{
	class RawDocumentQuery : public AbstractDocumentQuery
	{
	public:
		RawDocumentQuery(InMemoryDocumentSessionOperations& session, std::string raw_query)
			: AbstractDocumentQuery(session, {}, {}, false, {}, {}, {})
		{
			query_raw = std::move(raw_query);
		}
	};
}
