#include "stdafx.h"
#include "SubscriptionDoesNotExistException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionDoesNotExistException::SubscriptionDoesNotExistException(const std::string& message)
		: SubscriptionException(message)
	{}
}
