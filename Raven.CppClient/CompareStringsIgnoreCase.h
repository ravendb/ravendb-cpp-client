#pragma once
#include <cctype>

namespace ravendb::client::impl::utils
{
	struct CompareStringsIgnoreCase
	{
		static std::string to_lower_str(const std::string& str);

		bool operator()(const std::string& s1, const std::string& s2) const;
	};

	inline std::string CompareStringsIgnoreCase::to_lower_str(const std::string& str)
	{
		std::string temp{};
		std::transform(str.cbegin(), str.cend(), std::back_insert_iterator<std::string>(temp),
			[](std::string::value_type c) {return std::tolower(c); });
		return temp;
	}

	inline bool CompareStringsIgnoreCase::operator()(const std::string& s1, const std::string& s2) const
	{
		return to_lower_str(s1) < to_lower_str(s2);
	}
}
