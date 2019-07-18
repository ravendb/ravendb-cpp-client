#include "stdafx.h"
#include "ResponseTimeInformation.h"

namespace ravendb::client::documents::session
{
	void ResponseTimeInformation::compute_server_total()
	{
		total_server_duration = std::accumulate(duration_break_down.cbegin(), duration_break_down.cend(),
			std::chrono::milliseconds(0),
			[](const std::chrono::milliseconds& total, const ResponseTimeItem& time_item)
		{
			return total + time_item.duration;
		});
	}
}
