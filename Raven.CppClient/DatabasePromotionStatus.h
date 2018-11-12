#pragma once
#include "stdafx.h"

namespace ravendb::client::serverwide::operations
{
	enum class DatabasePromotionStatus
	{
		WAITING_FOR_FIRST_PROMOTION,
		NOT_RESPONDING,
		INDEX_NOT_UP_TO_DATE,
		CHANGE_VECTOR_NOT_MERGED,
		WAITING_FOR_RESPONSE,
		OK
	};
}
