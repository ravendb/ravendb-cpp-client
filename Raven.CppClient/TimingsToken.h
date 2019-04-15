#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class TimingsToken : public QueryToken
	{
	private:
		TimingsToken() = default;

	public:
		~TimingsToken() override = default;

		static const std::shared_ptr<TimingsToken> INSTANCE;

		void write_to(std::ostringstream& writer) const override;
	};
}
