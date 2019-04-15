#pragma once
#include "MoreLikeThisToken.h"
#include "MoreLikeThisOptions.h"

namespace ravendb::client::documents::queries::more_like_this
{
	class MoreLikeThisScope
	{
	private:
		std::shared_ptr<session::tokens::MoreLikeThisToken> _token{};
		const std::function<std::string(nlohmann::json)> _add_query_parameter;
		const std::optional<std::function<void()>> _on_dispose;

	public:
		MoreLikeThisScope(std::shared_ptr<session::tokens::MoreLikeThisToken> token,
			std::function<std::string(nlohmann::json)> add_query_parameter,
			std::optional<std::function<void()>> on_dispose);

		~MoreLikeThisScope();

		void with_options(const std::optional<MoreLikeThisOptions>& options);

		void with_document(const std::string& document);
	};
}
