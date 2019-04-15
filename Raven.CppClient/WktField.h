#pragma once
#include "DynamicSpatialField.h"

namespace ravendb::client::documents::queries::spatial
{
	class WktField : public DynamicSpatialField
	{
	public:
		const std::string wkt;

		~WktField() override = default;

		WktField(std::string wkt_param);

		std::string to_field(std::function<std::string(const std::string&, bool)> ensure_valid_field_name) const override;
	};
}
