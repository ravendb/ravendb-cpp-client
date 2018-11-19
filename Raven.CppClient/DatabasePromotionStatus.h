#pragma once
#include "stdafx.h"
#include <regex>

namespace ravendb::client::serverwide::operations::database_promotion_status
{
	enum class DatabasePromotionStatus : uint8_t
	{
		WAITING_FOR_FIRST_PROMOTION,
		NOT_RESPONDING,
		INDEX_NOT_UP_TO_DATE,
		CHANGE_VECTOR_NOT_MERGED,
		WAITING_FOR_RESPONSE,
		OK,
	};

	inline std::map<std::string, DatabasePromotionStatus> status_string_map =
	{
		{"WaitingForFirstPromotion", DatabasePromotionStatus::WAITING_FOR_FIRST_PROMOTION},
		{"NotResponding", DatabasePromotionStatus::NOT_RESPONDING},
		{"IndexNotUpToDate", DatabasePromotionStatus::INDEX_NOT_UP_TO_DATE},
		{"ChangeVectorNotMerged", DatabasePromotionStatus::CHANGE_VECTOR_NOT_MERGED},
		{"WaitingForResponse", DatabasePromotionStatus::WAITING_FOR_RESPONSE},
		{"OK",DatabasePromotionStatus::OK}
	};

	inline DatabasePromotionStatus from_string(const std::string& str)
	{
		const auto&& it = status_string_map.find(str);
		if(it == status_string_map.end())
		{
			throw std::invalid_argument("wrong DatabasePromotionStatus");
		}
		return it->second;
	}
}
