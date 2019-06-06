#pragma once
#include <map>

namespace ravendb::client::impl::utils
{
	struct CompareStringsLessThanIgnoreCase;
}

namespace ravendb::client::documents::session
{
	struct DocumentInfo;

	using DocumentsById = std::map<std::string, std::shared_ptr<DocumentInfo>, impl::utils::CompareStringsLessThanIgnoreCase>;
	
}
