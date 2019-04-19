#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class LoadToken : public QueryToken
	{
	private:
		LoadToken(std::optional<std::string> argument_param, std::optional<std::string> alias_param);

	public:
		~LoadToken() override = default;

		const std::optional<std::string> argument;
		const std::optional<std::string> alias;

		std::shared_ptr<LoadToken> create(std::optional<std::string> argument_param, std::optional<std::string> alias_param);

		void write_to(std::ostringstream& writer) const override;
	};
}
