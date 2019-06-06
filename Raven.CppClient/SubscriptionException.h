#pragma once
#include "RavenException.h"

namespace ravendb::client::exceptions::documents::subscriptions
{
	class SubscriptionException : public RavenException
	{
	public:
		~SubscriptionException() override = 0;

		explicit SubscriptionException(const std::string& message);
	};
}
