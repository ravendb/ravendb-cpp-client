#pragma once

namespace ravendb::client::documents::session
{
	enum class OrderingType
	{
		STRING,
		LONG,
		DOUBLE,
		ALPHA_NUMERIC
	};
}
