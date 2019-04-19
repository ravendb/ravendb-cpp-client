#pragma once
#include "MoreLikeThisBase.h"
#include "DocumentQuery.h"

namespace ravendb::client::documents::queries::more_like_this
{
	template<typename T>
	struct MoreLikeThisUsingDocumentForDocumentQuery : MoreLikeThisBase
	{
		std::function<void(std::shared_ptr<session::IFilterDocumentQueryBase<T, session::DocumentQuery<T>>>)> for_document_query{};

		~MoreLikeThisUsingDocumentForDocumentQuery() override = default;
	};
}
