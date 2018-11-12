#pragma once
#include "stdafx.h"

namespace ravendb::client::serverwide
{
	struct ScriptResolver
	{
		std::string script{};
		std::chrono::system_clock::time_point last_modified_time{};
	};
}