#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class OpenSubclauseToken : public QueryToken
	{
	private:
		OpenSubclauseToken() = default;

	public:
		~OpenSubclauseToken() override = default;

		static const std::shared_ptr<OpenSubclauseToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
