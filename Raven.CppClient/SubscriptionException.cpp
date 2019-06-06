#include "stdafx.h"
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionException::~SubscriptionException() = default;

	SubscriptionException::SubscriptionException(const std::string& message)
		: RavenException(message)
	{}
}
