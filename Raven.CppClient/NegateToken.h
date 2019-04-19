#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class NegateToken : public QueryToken
	{
	private:
		NegateToken() = default;

	public:
		~NegateToken() override = default;

		static const std::shared_ptr<NegateToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
