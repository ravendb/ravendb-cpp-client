#pragma once

namespace ravendb::client::tests
{
	struct Arr1
	{
		std::vector<std::string> str{};
	};

	inline void to_json(nlohmann::json& j, const Arr1& a)
	{}//purposefully left empty

	inline void from_json(const nlohmann::json& j, Arr1& a)
	{}//purposefully left empty

	struct Arr2
	{
		std::vector<Arr1> arr1;
	};

	inline void to_json(nlohmann::json& j, const Arr2& a)
	{}//purposefully left empty

	inline void from_json(const nlohmann::json& j, Arr2& a)
	{}//purposefully left empty
}