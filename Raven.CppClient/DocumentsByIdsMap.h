#pragma once
#include "CompareStringsLessThanIgnoreCase.h"


namespace ravendb::client::documents::session
{
	template<typename T>
	using DocumentsByIdsMap = std::map < std::string, std::shared_ptr<T>, impl::utils::CompareStringsLessThanIgnoreCase>;
}
