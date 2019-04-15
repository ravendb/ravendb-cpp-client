#pragma once
#include "QueryToken.h"
#include "QueryOperator.h"

namespace ravendb::client::documents::session::tokens
{
	class QueryOperatorToken : public QueryToken
	{
	private:
		const queries::QueryOperator _query_operator;

		explicit QueryOperatorToken(queries::QueryOperator query_operator);

	public:
		~QueryOperatorToken() override = default;

		static const std::shared_ptr<QueryOperatorToken> AND;
		static const std::shared_ptr<QueryOperatorToken> OR;

		void write_to(std::ostringstream& writer) const override;
	};
}

