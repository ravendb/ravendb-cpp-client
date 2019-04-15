#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class IntersectMarkerToken : public QueryToken
	{
	private:
		IntersectMarkerToken() = default;

	public:
		~IntersectMarkerToken() override = default;

		static const std::shared_ptr<IntersectMarkerToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
