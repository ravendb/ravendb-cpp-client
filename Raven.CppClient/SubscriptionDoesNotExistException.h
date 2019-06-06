#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionDoesNotExistException : public SubscriptionException
	{
	public:
		~SubscriptionDoesNotExistException() override = default;

		explicit SubscriptionDoesNotExistException(const std::string& message);
	};
}
