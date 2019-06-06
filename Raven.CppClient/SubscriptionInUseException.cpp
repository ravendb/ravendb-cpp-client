#include "stdafx.h"
#include "SubscriptionInUseException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionInUseException::SubscriptionInUseException(const std::string& message)
		: SubscriptionException(message)
	{}
}
