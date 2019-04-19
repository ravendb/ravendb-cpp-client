#pragma once

namespace ravendb::client::documents::indexes::spatial
{
	enum class AutoSpatialMethodType
	{
		POINT,
		WKT,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(AutoSpatialMethodType,
		{
			{AutoSpatialMethodType::UNSET, nullptr},
			{AutoSpatialMethodType::POINT, "Point"},
			{AutoSpatialMethodType::WKT, "Wkt"}
		});
}
