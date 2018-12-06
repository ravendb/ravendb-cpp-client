#pragma once
#include "stdafx.h"
#include <regex>

namespace ravendb::client::serverwide::operations::database_promotion_status
{
	enum class DatabasePromotionStatus
	{
		WAITING_FOR_FIRST_PROMOTION,
		NOT_RESPONDING,
		INDEX_NOT_UP_TO_DATE,
		CHANGE_VECTOR_NOT_MERGED,
		WAITING_FOR_RESPONSE,
		OK,
		INVALID_VALUE
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(DatabasePromotionStatus,
		{
		{DatabasePromotionStatus::INVALID_VALUE, nullptr},
		{DatabasePromotionStatus::WAITING_FOR_FIRST_PROMOTION, "WaitingForFirstPromotion"},
		{DatabasePromotionStatus::NOT_RESPONDING , "NotResponding"},
		{DatabasePromotionStatus::INDEX_NOT_UP_TO_DATE, "IndexNotUpToDate"},
		{DatabasePromotionStatus::CHANGE_VECTOR_NOT_MERGED, "ChangeVectorNotMerged"},
		{DatabasePromotionStatus::WAITING_FOR_RESPONSE, "WaitingForResponse"},
		{DatabasePromotionStatus::OK, "OK"}
		});
}
