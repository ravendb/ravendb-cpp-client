#pragma once

namespace ravendb::client::documents::indexes
{
	enum class FieldStorage
	{
		YES,
		NO,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(FieldStorage,
	{
		{FieldStorage::UNSET, nullptr},
		{FieldStorage::YES, "Yes"},
		{FieldStorage::NO, "No"}
	});
}
