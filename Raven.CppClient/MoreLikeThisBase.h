#pragma once
#include "MoreLikeThisOptions.h"

namespace ravendb::client::documents::queries::more_like_this
{
	struct MoreLikeThisBase
	{
		virtual ~MoreLikeThisBase() = 0;

		std::optional<MoreLikeThisOptions> options{};
	};

	inline MoreLikeThisBase::~MoreLikeThisBase() = default;
}
