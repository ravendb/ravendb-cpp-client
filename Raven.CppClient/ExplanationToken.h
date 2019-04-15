#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class ExplanationToken : public QueryToken
	{
	private:
		const std::optional<std::string> _options_parameter_name;

		explicit ExplanationToken(std::optional<std::string> options_parameter_name);

	public:
		~ExplanationToken() override = default;

		static std::shared_ptr<ExplanationToken> create(std::optional<std::string> options_parameter_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
