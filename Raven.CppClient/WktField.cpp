#include "stdafx.h"
#include "WktField.h"

namespace ravendb::client::documents::queries::spatial
{
	WktField::WktField(std::string wkt_param)
		: wkt(wkt_param)
	{}

	std::string WktField::to_field(std::function<std::string(const std::string&, bool)> ensure_valid_field_name) const
	{
		std::ostringstream res{};

		res << "spatial.wkt(" << ensure_valid_field_name(wkt, false) << ")";

		return res.str();
	}
}
