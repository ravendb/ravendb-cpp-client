#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class SuggestToken : public QueryToken
	{
	private:
		const std::string _field_name;
		const std::string _term_parameter_name;
		const std::optional<std::string> _options_parameter_name;

		SuggestToken(std::string field_name,
			std::string term_parameter_name,
			std::optional<std::string> options_parameter_name);

	public:
		~SuggestToken() override = default;

		static std::shared_ptr<SuggestToken> create(std::string field_name,
			std::string term_parameter_name,
			std::optional<std::string> options_parameter_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
