#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class HighlightingToken : public QueryToken
	{
	private:
		const std::optional<std::string> _field_name;
		const int32_t _fragment_length;
		const int32_t _fragment_count;
		std::optional<std::string> _options_parameter_name;

		HighlightingToken(std::optional<std::string> field_name, int32_t fragment_length, int32_t fragment_count,
			std::optional<std::string> options_parameter_name);

	public:
		~HighlightingToken() override = default;

		static std::shared_ptr<HighlightingToken> create(std::optional<std::string> field_name, int32_t fragment_length, int32_t fragment_count,
			std::optional<std::string> options_parameter_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
