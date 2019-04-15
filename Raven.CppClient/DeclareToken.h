#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class DeclareToken : public QueryToken
	{
	private:
		const std::string _name;
		const std::string _body;
		const std::optional<std::string> _parameters;

		DeclareToken(std::string name, std::string body, std::optional<std::string> parameters);

	public:
		~DeclareToken() override = default;

		static std::shared_ptr<DeclareToken> create(std::string name, std::string body, std::optional<std::string> parameters = {});

		void write_to(std::ostringstream& writer) const override;
	};
}
