#include "stdafx.h"
#include "GroupByCountToken.h"

namespace ravendb::client::documents::session::tokens
{
	GroupByCountToken::GroupByCountToken(std::optional<std::string> filed_name)
		: _field_name(std::move(filed_name))
	{}

	std::shared_ptr<GroupByCountToken> GroupByCountToken::create(std::optional<std::string> field_name)
	{
		return std::shared_ptr<GroupByCountToken>(new GroupByCountToken(std::move(field_name)));
	}

	void GroupByCountToken::write_to(std::ostringstream& writer) const
	{
		writer << "count()";
		if(!_field_name)
		{
			return;
		}
		writer << " as " << *_field_name;
	}
}
