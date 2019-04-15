#include "stdafx.h"
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	const std::set<std::string> QueryToken::RQL_KEYWORDS =
	{
			"as",
			"select",
			"where",
			"load",
			"group",
			"order",
			"include"
	};

	void QueryToken::write_field(std::ostringstream& writer, const std::optional<std::string>& field)
	{
		if(!field)
		{
			return;
		}
		const bool is_keyword = RQL_KEYWORDS.find(*field) != RQL_KEYWORDS.end();
		if (is_keyword)
		{
			writer << "'";
		}
		writer << *field;
		if (is_keyword)
		{
			writer << "'";
		}
	}

	QueryToken::~QueryToken() = default;
}