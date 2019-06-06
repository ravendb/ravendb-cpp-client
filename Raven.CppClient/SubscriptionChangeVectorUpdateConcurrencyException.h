#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionChangeVectorUpdateConcurrencyException : public SubscriptionException
	{
	public:
		~SubscriptionChangeVectorUpdateConcurrencyException() override = default;

		explicit SubscriptionChangeVectorUpdateConcurrencyException(const std::string& message);
	};
}
