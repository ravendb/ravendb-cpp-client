#pragma once
#include "utils.h"
#include "DocumentInfo.h"

namespace ravendb::client::documents::session
{
	using DocumentsById = std::map<std::string, std::shared_ptr<DocumentInfo>, impl::utils::CompareStringsIgnoreCase>;
	
}
