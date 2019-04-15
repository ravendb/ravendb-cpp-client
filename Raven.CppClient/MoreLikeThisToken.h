#pragma once
#include "WhereToken.h"

namespace ravendb::client::documents::session::tokens
{
	class MoreLikeThisToken : WhereToken
	{
	public:
		std::optional<std::string> document_parameter_name{};
		std::optional<std::string> options_parameter_name{};

		std::list<std::shared_ptr<QueryToken>> where_tokens{};

	private:
		MoreLikeThisToken();

	public:
		~MoreLikeThisToken() override = default;

		static std::shared_ptr<MoreLikeThisToken> create();

		void write_to(std::ostringstream& writer) const override;
	};
}
