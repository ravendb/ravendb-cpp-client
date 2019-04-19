#include "stdafx.h"
#include "GroupByKeyToken.h"

namespace ravendb::client::documents::session::tokens
{
	GroupByKeyToken::GroupByKeyToken(std::optional<std::string> field_name, std::optional<std::string> projected_name)
		: _field_name(std::move(field_name))
		, _projected_name(std::move(projected_name))
	{}

	std::shared_ptr<GroupByKeyToken> GroupByKeyToken::create(std::optional<std::string> field_name,
		std::optional<std::string> projected_name)
	{
		return std::shared_ptr<GroupByKeyToken>(new GroupByKeyToken(std::move(field_name), std::move(projected_name)));
	}

	void GroupByKeyToken::write_to(std::ostringstream& writer) const
	{
		write_field(writer, _field_name ? _field_name : "key()");

		if(!_projected_name || _projected_name == _field_name)
		{
			return;
		}

		writer << " as " << *_projected_name;
	}
}
