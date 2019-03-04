#pragma once

namespace ravendb::client::http
{
	enum class ReadBalanceBehavior
	{
		NONE,
		ROUND_ROBIN,
		FASTEST_NODE
	};
}
