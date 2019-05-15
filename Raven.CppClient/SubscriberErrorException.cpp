#include "stdafx.h"
#include "SubscriberErrorException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriberErrorException::SubscriberErrorException(const std::string& message)
		: SubscriptionException(message)
	{}
}
