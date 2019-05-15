#include "stdafx.h"
#include "SubscriptionClosedException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionClosedException::SubscriptionClosedException(const std::string& message)
		: SubscriptionException(message)
	{}
}
