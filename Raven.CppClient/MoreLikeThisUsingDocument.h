#pragma once
#include "MoreLikeThisBase.h"

namespace ravendb::client::documents::queries::more_like_this
{
	struct MoreLikeThisUsingDocument : MoreLikeThisBase
	{
		std::string document_json{};
		
		~MoreLikeThisUsingDocument() override = default;

		explicit MoreLikeThisUsingDocument(std::string document_json_param)
			: document_json(std::move(document_json_param))
		{}
	};
}
