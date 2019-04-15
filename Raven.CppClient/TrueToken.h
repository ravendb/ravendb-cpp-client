#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class TrueToken : public QueryToken
	{
	private:
		TrueToken() = default;

	public:
		~TrueToken() override = default;

		static const std::shared_ptr<TrueToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
