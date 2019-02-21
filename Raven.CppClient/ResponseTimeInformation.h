#pragma once

namespace ravendb::client::documents::session
{
	struct ResponseTimeInformation
	{
		struct ResponseTimeItem;

		std::chrono::milliseconds total_server_duration{0};
		std::chrono::milliseconds total_client_duration{0};

		std::vector<ResponseTimeItem> duration_break_down{};

		void compute_server_total();
	};

	struct ResponseTimeInformation::ResponseTimeItem
	{
		std::string url{};
		std::chrono::milliseconds duration{};
	};

	inline void ResponseTimeInformation::compute_server_total()
	{
		total_server_duration = std::accumulate(duration_break_down.cbegin(), duration_break_down.cend(),
			std::chrono::milliseconds(0),
			[](const std::chrono::milliseconds& total, const ResponseTimeItem& time_item)
		{
			return total + time_item.duration;
		});
	}
}