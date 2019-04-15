#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class DistinctToken : public QueryToken
	{
	private:
		DistinctToken() = default;

	public:
		~DistinctToken() override = default;

		static const std::shared_ptr<DistinctToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
