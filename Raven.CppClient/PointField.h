#pragma once
#include "DynamicSpatialField.h"

namespace ravendb::client::documents::queries::spatial
{
	class PointField : public DynamicSpatialField
	{
	public:
		const std::string latitude;
		const std::string longitude;
	
		~PointField() override = default;

		PointField(std::string latitude_param, std::string longitude_param);

		std::string to_field(std::function<std::string(const std::string&, bool)> ensure_valid_field_name) const override;
	};
}
