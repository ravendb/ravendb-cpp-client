#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriberErrorException : SubscriptionException
	{
	public:
		~SubscriberErrorException() override = default;

		explicit SubscriberErrorException(const std::string& message);
	};
}
