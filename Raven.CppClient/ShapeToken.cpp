#include "stdafx.h"
#include "ShapeToken.h"

namespace ravendb::client::documents::session::tokens
{
	ShapeToken::ShapeToken(std::string shape)
		: _shape(shape)
	{}

	std::shared_ptr<ShapeToken> ShapeToken::circle(const std::string& radius_parameter_name,
		const std::string& latitude_parameter_name, const std::string& longitude_parameter_name,
		const std::optional<indexes::spatial::SpatialUnits>& radius_units)
	{
		std::ostringstream shape{};

		if(!radius_units)
		{
			shape << "spatial.circle($" << radius_parameter_name << ", $" <<
				latitude_parameter_name << ", $" << longitude_parameter_name + ")";
		}
		else if (*radius_units == indexes::spatial::SpatialUnits::KILOMETERS)
		{
			shape << "spatial.circle($" << radius_parameter_name << ", $" <<
				latitude_parameter_name << ", $" << longitude_parameter_name + ", 'Kilometers')";
		}
		else if (*radius_units == indexes::spatial::SpatialUnits::MILES)
		{
			shape << "spatial.circle($" << radius_parameter_name << ", $" <<
				latitude_parameter_name << ", $" << longitude_parameter_name + ", 'Miles')";
		}

		return std::shared_ptr<ShapeToken>(new ShapeToken(shape.str()));
	}

	std::shared_ptr<ShapeToken> ShapeToken::wkt(const std::string& shape_wkt_parameter_name,
		const std::optional<indexes::spatial::SpatialUnits>& units)
	{
		std::ostringstream shape{};

		if(!units)
		{
			shape << "spatial.wkt($" << shape_wkt_parameter_name << ")";
		}
		else if (*units == indexes::spatial::SpatialUnits::KILOMETERS)
		{
			shape << "spatial.wkt($" << shape_wkt_parameter_name << ", 'Kilometers')";
		}
		else if (*units == indexes::spatial::SpatialUnits::MILES)
		{
			shape << "spatial.wkt($" << shape_wkt_parameter_name << ", 'Miles')";
		}

		return std::shared_ptr<ShapeToken>(new ShapeToken(shape.str()));
	}

	void ShapeToken::write_to(std::ostringstream& writer) const
	{
		writer << _shape;
	}
}
