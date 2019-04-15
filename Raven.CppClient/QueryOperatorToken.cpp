#include "stdafx.h"
#include "QueryOperatorToken.h"

namespace ravendb::client::documents::session::tokens
{
	QueryOperatorToken::QueryOperatorToken(queries::QueryOperator query_operator)
		: _query_operator(query_operator)
	{}

	void QueryOperatorToken::write_to(std::ostringstream& writer) const
	{
		if(_query_operator == queries::QueryOperator::AND)
		{
			writer << "and";
		}
		else if (_query_operator == queries::QueryOperator::OR)
		{
			writer << "or";
		}
	}

	const std::shared_ptr<QueryOperatorToken> QueryOperatorToken::AND = 
		std::shared_ptr<QueryOperatorToken>(new QueryOperatorToken(queries::QueryOperator::AND));

	const std::shared_ptr<QueryOperatorToken> QueryOperatorToken::OR = 
		std::shared_ptr<QueryOperatorToken>(new QueryOperatorToken(queries::QueryOperator::OR));


}
