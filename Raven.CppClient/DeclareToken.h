#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class DeclareToken : public QueryToken
	{
	private:

		const std::string _name;
		const std::string _body;
		const std::string _parameters;

	public:
		~DeclareToken() override = default;

		DeclareToken(std::string name, std::string body, std::string parameters = {});

		void write_to(std::ostringstream& oss) const override;
	};
}
