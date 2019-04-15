#include "stdafx.h"
#include "DocumentQueryHelper.h"
#include "OpenSubclauseToken.h"
#include "CloseSubclauseToken.h"
#include "IntersectMarkerToken.h"

namespace ravendb::client::documents::session
{
	void DocumentQueryHelper::add_space_if_needed(std::shared_ptr<tokens::QueryToken> previous_token,
		std::shared_ptr<tokens::QueryToken> current_token, std::ostringstream& writer)
	{
		if(!previous_token)
		{
			return;
		}

		if(std::dynamic_pointer_cast<tokens::OpenSubclauseToken>(previous_token) ||
			std::dynamic_pointer_cast<tokens::CloseSubclauseToken>(current_token) ||
			std::dynamic_pointer_cast<tokens::IntersectMarkerToken>(current_token))
		{
			return;
		}
		writer << " ";
	}
}
