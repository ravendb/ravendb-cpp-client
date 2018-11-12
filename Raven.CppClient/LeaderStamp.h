#pragma once
#include "stdafx.h"

namespace  ravendb::client::serverwide
{
	struct LeaderStamp
	{
		int64_t index = -1;
		int64_t term = -1;
		int64_t leadersTicks = -1;
	};
}