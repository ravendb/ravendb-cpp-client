#pragma once
#include "IServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	//Represents server operation which doesn't return any response
	using IVoidServerOperation = IServerOperation<void>;
}
