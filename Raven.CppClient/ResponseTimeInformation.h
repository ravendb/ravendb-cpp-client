#pragma once
#include <chrono>

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
}