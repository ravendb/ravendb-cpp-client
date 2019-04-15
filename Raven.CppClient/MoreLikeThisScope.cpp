#include "stdafx.h"
#include "MoreLikeThisScope.h"

namespace ravendb::client::documents::queries::more_like_this
{
	MoreLikeThisScope::MoreLikeThisScope(std::shared_ptr<session::tokens::MoreLikeThisToken> token,
		std::function<std::string(nlohmann::json)> add_query_parameter, 
		std::optional<std::function<void()>> on_dispose)
		: _token(token)
		, _add_query_parameter(add_query_parameter)
		, _on_dispose(on_dispose)
	{}

	MoreLikeThisScope::~MoreLikeThisScope()
	{
		if (_on_dispose.has_value() && *_on_dispose)
		{
			try
			{
				(*_on_dispose)();
			}
			catch (...)
			{}
		}
	}

	void MoreLikeThisScope::with_options(const std::optional<MoreLikeThisOptions>& options)
	{
		if(options)
		{
			_token->options_parameter_name = _add_query_parameter(*options);
		}
	}

	void MoreLikeThisScope::with_document(const std::string& document)
	{
		_token->document_parameter_name = _add_query_parameter(document);
	}
}
