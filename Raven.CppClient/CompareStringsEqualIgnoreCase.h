#pragma once
#include "utils.h"

namespace ravendb::client::impl::utils
{
	struct CompareStringsEqualIgnoreCase
	{
		bool operator()(const std::string& s1, const std::string& s2) const;
	};

	inline bool CompareStringsEqualIgnoreCase::operator()(const std::string& s1, const std::string& s2) const
	{
		return to_lower_str(s1) == to_lower_str(s2);
	}
}
