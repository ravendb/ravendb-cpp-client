#include "stdafx.h"
#include "GroupByToken.h"

namespace ravendb::client::documents::session::tokens
{
	GroupByToken::GroupByToken(std::string field_name, queries::GroupByMethod method)
		: _filed_name(std::move(field_name))
		, _method(method)
	{}

	std::shared_ptr<GroupByToken> GroupByToken::create(std::string field_name, queries::GroupByMethod method)
	{
		return std::shared_ptr<GroupByToken>(new GroupByToken(std::move(field_name), method));
	}

	void GroupByToken::write_to(std::ostringstream& writer) const
	{
		if(_method != queries::GroupByMethod::NONE)
		{
			writer << "Array(";
		}
		write_field(writer, _filed_name);
		if (_method != queries::GroupByMethod::NONE)
		{
			writer << ")";
		}
	}
}
