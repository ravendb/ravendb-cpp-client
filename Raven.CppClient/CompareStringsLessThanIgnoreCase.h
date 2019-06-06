#pragma once
#include "utils.h"

namespace ravendb::client::impl::utils
{
	struct CompareStringsLessThanIgnoreCase
	{
		bool operator()(const std::string& s1, const std::string& s2) const;
	};

	inline bool CompareStringsLessThanIgnoreCase::operator()(const std::string& s1, const std::string& s2) const
	{
		return to_lower_str(s1) < to_lower_str(s2);
	}
}
