#pragma once

namespace ravendb::client::documents::queries::spatial
{
	class DynamicSpatialField
	{
	public:
		virtual ~DynamicSpatialField() = 0;

		virtual std::string to_field(std::function<std::string(const std::string&, bool)> ensure_valid_field_name) const = 0;
	};

	inline DynamicSpatialField::~DynamicSpatialField() = default;
}
