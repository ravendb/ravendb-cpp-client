#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionClosedException : public SubscriptionException
	{
	public:
		~SubscriptionClosedException() override = default;

		explicit SubscriptionClosedException(const std::string& message);
	};
}
