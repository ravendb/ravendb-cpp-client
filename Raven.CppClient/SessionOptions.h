#pragma once
#include "TransactionMode.h"

namespace ravendb::client::http
{
	class RequestExecutor;
}

namespace ravendb::client::documents::session
{
	struct SessionOptions
	{
		std::string database{};
		bool no_tracking{ false };
		bool no_caching{ false };
		std::shared_ptr<http::RequestExecutor> request_executor{};
		TransactionMode transaction_mode = TransactionMode::UNSET;
	};
}
