#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session
{
	class DocumentQueryHelper
	{
	public:
		DocumentQueryHelper() = delete;

		static void add_space_if_needed(std::shared_ptr<tokens::QueryToken> previous_token,
			std::shared_ptr<tokens::QueryToken> current_token,
			std::ostringstream& writer);
	};
}
