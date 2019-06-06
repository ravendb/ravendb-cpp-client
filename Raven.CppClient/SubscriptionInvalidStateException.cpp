#include "stdafx.h"
#include "SubscriptionInvalidStateException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionInvalidStateException::SubscriptionInvalidStateException(const std::string& message)
		: SubscriptionException(message)
	{}
}
