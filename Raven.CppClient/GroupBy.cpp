#include "stdafx.h"
#include "GroupBy.h"

namespace ravendb::client::documents::queries
{
	const std::string& GroupBy::get_field() const
	{
		return _field;
	}

	GroupByMethod GroupBy::get_method() const
	{
		return _method;
	}

	GroupBy GroupBy::field(std::string field_name)
	{
		auto group_by = GroupBy();
		group_by._field = std::move(field_name);
		group_by._method = GroupByMethod::NONE;

		return group_by;
	}

	GroupBy GroupBy::array(std::string field_name)
	{
		auto group_by = GroupBy();
		group_by._field = std::move(field_name);
		group_by._method = GroupByMethod::ARRAY;

		return group_by;
	}
}
