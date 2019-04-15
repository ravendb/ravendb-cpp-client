#include "stdafx.h"
#include "QueryData.h"

namespace ravendb::client::documents::queries
{
	QueryData::QueryData(std::vector<std::string> fields_param, std::vector<std::string> projections_param,
		std::optional<std::string> from_alias_param, std::shared_ptr<session::tokens::DeclareToken> declare_token_param,
		std::vector<std::shared_ptr<session::tokens::LoadToken>> load_tokens_param, bool is_custom_function_param)
		: fields(std::move(fields_param))
		, projections(std::move(projections_param))
		, from_alias(std::move(from_alias_param))
		, declare_token(declare_token_param)
		, load_tokens(std::move(load_tokens_param))
		, is_custom_function(is_custom_function_param)
	{}

	QueryData QueryData::custom_function(std::string alias, std::string func)
	{
		return QueryData({ std::move(func) }, {}, std::move(alias),
			{}, {}, true);
	}
}
