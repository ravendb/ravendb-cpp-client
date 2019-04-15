#pragma once
#include "QueryToken.h"
#include "SpatialUnits.h"

namespace ravendb::client::documents::session::tokens
{
	class ShapeToken : public QueryToken
	{
	private:
		std::string _shape{};

		explicit ShapeToken(std::string shape);

	public:
		~ShapeToken() override = default;

		static std::shared_ptr<ShapeToken> circle(const std::string& radius_parameter_name,
			const std::string& latitude_parameter_name, const std::string& longitude_parameter_name,
			const std::optional<indexes::spatial::SpatialUnits>& radius_units);

		static std::shared_ptr<ShapeToken> wkt(const std::string& shape_wkt_parameter_name,
			const std::optional<indexes::spatial::SpatialUnits>& units);

		void write_to(std::ostringstream& writer) const override;
	};
}
