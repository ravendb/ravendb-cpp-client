#include "stdafx.h"
#include "SubscriptionDoesNotBelongToNodeException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionDoesNotBelongToNodeException::SubscriptionDoesNotBelongToNodeException(const std::string& message)
		: SubscriptionException(message)
	{}
}
