#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionInUseException : public SubscriptionException
	{
	public:
		~SubscriptionInUseException() override = default;

		explicit SubscriptionInUseException(const std::string& message);
	};
}
