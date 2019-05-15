#pragma once
#include "SubscriptionException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionDoesNotBelongToNodeException : SubscriptionException
	{
	public:
		std::string appropriate_node{};

		~SubscriptionDoesNotBelongToNodeException() override = default;

		explicit SubscriptionDoesNotBelongToNodeException(const std::string& message);
	};
}
