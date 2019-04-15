#pragma once

namespace ravendb::client::documents::session
{
	struct GroupByField
	{
		std::string field_name{};
		std::optional<std::string> projected_name{};

		explicit GroupByField(std::string field_name_param, std::optional<std::string> projected_name = {});
	};

	inline GroupByField::GroupByField(std::string field_name_param, std::optional<std::string> projected_name_param)
		: field_name(std::move(field_name_param))
		, projected_name(std::move(projected_name_param))
	{}
}
