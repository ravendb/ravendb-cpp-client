#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class CloseSubclauseToken : public QueryToken
	{
	private:
		CloseSubclauseToken() = default;

	public:
		~CloseSubclauseToken() override = default;

		static const std::shared_ptr<CloseSubclauseToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
