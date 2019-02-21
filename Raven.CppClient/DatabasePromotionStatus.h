#pragma once

namespace ravendb::client::serverwide::operations
{
	enum class DatabasePromotionStatus
	{
		WAITING_FOR_FIRST_PROMOTION,
		NOT_RESPONDING,
		INDEX_NOT_UP_TO_DATE,
		CHANGE_VECTOR_NOT_MERGED,
		WAITING_FOR_RESPONSE,
		OK,
		INVALID_VALUE = -1
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
