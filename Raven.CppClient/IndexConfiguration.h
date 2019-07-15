#pragma once
#include <string>
#include <unordered_map>

namespace ravendb::client::documents::indexes
{
	using IndexConfiguration = std::unordered_map<std::string, std::string>;
}
