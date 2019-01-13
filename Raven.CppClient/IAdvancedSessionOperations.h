#pragma once

namespace ravendb::client::documents::session
{
	struct IAdvancedSessionOperations
	{
		virtual ~IAdvancedSessionOperations() = 0;
	};

	inline IAdvancedSessionOperations::~IAdvancedSessionOperations() = default;
}
