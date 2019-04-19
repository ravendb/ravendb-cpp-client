#include "stdafx.h"
#include "HighlightingToken.h"

namespace ravendb::client::documents::session::tokens
{
	HighlightingToken::HighlightingToken(std::optional<std::string> field_name, int32_t fragment_length,
		int32_t fragment_count, std::optional<std::string> options_parameter_name)
		: _field_name(std::move(field_name))
		, _fragment_length(fragment_length)
		, _fragment_count(fragment_count)
		, _options_parameter_name(std::move(options_parameter_name))
	{}

	std::shared_ptr<HighlightingToken> HighlightingToken::create(std::optional<std::string> field_name,
		int32_t fragment_length, int32_t fragment_count, std::optional<std::string> options_parameter_name)
	{
		return std::shared_ptr<HighlightingToken>(new HighlightingToken(std::move(field_name),
			fragment_length, fragment_count, std::move(options_parameter_name)));
	}

	void HighlightingToken::write_to(std::ostringstream& writer) const
	{
		writer << "highlight(";

		write_field(writer, _field_name);

		writer << "," << _fragment_length << "," << _fragment_count;

		if(_options_parameter_name)
		{
			writer << ",$" << *_options_parameter_name;
		}
		writer << ")";
	}
}
