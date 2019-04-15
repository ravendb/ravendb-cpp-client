#pragma once
#include "DeclareToken.h"
#include "LoadToken.h"

namespace ravendb::client::documents::queries
{
	struct QueryData
	{
		std::vector<std::string> fields{};
		std::vector<std::string> projections{};
		std::optional<std::string> from_alias{};
		std::shared_ptr<session::tokens::DeclareToken> declare_token{};
		std::vector<std::shared_ptr<session::tokens::LoadToken>> load_tokens;
		bool is_custom_function = false;
		bool is_map_reduce = false;

		QueryData(std::vector<std::string> fields_param, std::vector<std::string> projections_param,
			std::optional<std::string> from_alias_param = {},
			std::shared_ptr<session::tokens::DeclareToken> declare_token_param = {},
			std::vector<std::shared_ptr<session::tokens::LoadToken>> load_tokens_param = {},
			bool is_custom_function_param = false);

		static QueryData custom_function(std::string alias, std::string func);
	};
}
