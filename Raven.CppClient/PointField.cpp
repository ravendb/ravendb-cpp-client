#include "stdafx.h"
#include "PointField.h"

namespace ravendb::client::documents::queries::spatial
{
	PointField::PointField(std::string latitude_param, std::string longitude_param)
		: latitude(std::move(latitude_param))
		, longitude(std::move(longitude_param))
	{}

	std::string PointField::to_field(std::function<std::string(const std::string&, bool)> ensure_valid_field_name) const
	{
		std::ostringstream res{};

		res << "spatial.point(" << ensure_valid_field_name(latitude, false) << ", " <<
			ensure_valid_field_name(longitude, false) << ")";

		return res.str();
	}
}
