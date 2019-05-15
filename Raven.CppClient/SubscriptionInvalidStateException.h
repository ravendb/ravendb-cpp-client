#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionInvalidStateException : public SubscriptionException
	{
	public:
		~SubscriptionInvalidStateException() override = default;

		explicit SubscriptionInvalidStateException(const std::string& message);
	};
}
