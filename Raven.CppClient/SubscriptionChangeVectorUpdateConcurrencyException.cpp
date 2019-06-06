#include "stdafx.h"
#include "SubscriptionChangeVectorUpdateConcurrencyException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	SubscriptionChangeVectorUpdateConcurrencyException::SubscriptionChangeVectorUpdateConcurrencyException(
		const std::string& message)
		:	SubscriptionException(message)
	{}
}
