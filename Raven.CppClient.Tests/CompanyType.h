#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	enum class CompanyType
	{
		PUBLIC,
		PRIVATE,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(CompanyType,
		{
			{CompanyType::UNSET, nullptr},
			{CompanyType::PUBLIC, "Public"},
			{CompanyType::PRIVATE, "Private"}
		});

}

