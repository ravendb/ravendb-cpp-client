#pragma once
#include "stdafx.h"
#include "ScriptResolver.h"

namespace ravendb::client::serverwide
{
	struct ConflictSolver
	{
		std::map<std::string, ScriptResolver> resolve_by_collection{};
		bool resolve_to_latest = true;//am I right ?
	};
}
