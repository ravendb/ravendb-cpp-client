#include "stdafx.h"
#include "GroupBySumToken.h"

namespace ravendb::client::documents::session::tokens
{
	GroupBySumToken::GroupBySumToken(std::string filed_name, std::optional<std::string> projected_name)
		: _field_name(std::move(filed_name))
		, _projected_name(std::move(projected_name))
	{}

	std::shared_ptr<GroupBySumToken> GroupBySumToken::create(std::string filed_name,
		std::optional<std::string> projected_name)
	{
		return std::shared_ptr<GroupBySumToken>(new GroupBySumToken(std::move(filed_name), std::move(projected_name)));
	}

	void GroupBySumToken::write_to(std::ostringstream& writer) const
	{
		writer << "sum(" << _field_name << ")";
		if(!_projected_name)
		{
			return;
		}
		writer << " as " << *_projected_name;
	}
}
