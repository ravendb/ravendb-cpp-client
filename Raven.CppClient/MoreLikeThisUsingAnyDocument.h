#pragma once
#include "MoreLikeThisBase.h"

namespace ravendb::client::documents::queries::more_like_this
{
	struct MoreLikeThisUsingAnyDocument : MoreLikeThisBase
	{
		~MoreLikeThisUsingAnyDocument() override = default;
	};
}
