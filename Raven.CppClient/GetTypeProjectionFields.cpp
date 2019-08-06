#include "stdafx.h"
#include "GetTypeProjectionFields.h"

namespace ravendb::client::impl::utils
{
	std::shared_mutex GetTypeProjectionFields::_projection_fields_mutex{};
	std::unordered_map<std::type_index, std::vector<std::string>> GetTypeProjectionFields::_projection_fields{};
}
