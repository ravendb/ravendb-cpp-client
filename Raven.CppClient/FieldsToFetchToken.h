#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	struct FieldsToFetchToken : public QueryToken
	{
		const std::vector<std::optional<std::string>> _fields_to_fetch;
		const std::optional<std::vector<std::string>> _projections;
		const bool _custom_function;
		const std::optional<std::string> _source_alias;


		~FieldsToFetchToken() override;
		
		FieldsToFetchToken(std::vector<std::optional<std::string>> fields_to_fetch,
			std::optional<std::vector<std::string>> projections,
			bool custom_function, std::optional<std::string> source_alias);

		void write_to(std::ostringstream& oss) const override;
	};
}
