#include "stdafx.h"
#include "MoreLikeThisToken.h"
#include "DocumentQueryHelper.h"
#include <iterator>

namespace ravendb::client::documents::session::tokens
{
	MoreLikeThisToken::MoreLikeThisToken() = default;

	std::shared_ptr<MoreLikeThisToken> MoreLikeThisToken::create()
	{
		return std::shared_ptr<MoreLikeThisToken>(new MoreLikeThisToken());
	}

	void MoreLikeThisToken::write_to(std::ostringstream& writer) const
	{
		writer << "moreLikeThis(";

		if(!document_parameter_name)
		{
			for(auto it = where_tokens.begin(); it != where_tokens.end(); ++it)
			{
				DocumentQueryHelper::add_space_if_needed(it != where_tokens.begin() ?
					*std::prev(it) : std::shared_ptr<QueryToken>(), *it, writer);
				(*it)->write_to(writer);
			}
		}
		else
		{
			writer << "$" << *document_parameter_name;
		}

		if(!options_parameter_name)
		{
			writer << ")";
		}
		else
		{
			writer << ", $" << *options_parameter_name << ")";
		}
	}
}
