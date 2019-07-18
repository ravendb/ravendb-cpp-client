#include "stdafx.h"
#include "GetRequest.h"

namespace ravendb::client::documents::commands::multi_get
{
	std::string GetRequest::get_url_and_query() const
	{
		if (!query)
		{
			return url;
		}

		return url + ((!query->empty() && query->front() == '?') ? "" : "?") + *query;
	}
}
