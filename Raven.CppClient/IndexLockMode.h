#pragma once

namespace ravendb::client::documents::indexes
{
	enum class IndexLockMode
	{
		Unlock,
		LockedIgnore,
		LockedError
	};
}
