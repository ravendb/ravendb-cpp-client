#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class GroupByCountToken : public QueryToken
	{
		const std::optional<std::string> _field_name;

		explicit GroupByCountToken(std::optional<std::string> filed_name);

	public:
		~GroupByCountToken() override = default;

		static std::shared_ptr<GroupByCountToken> create(std::optional<std::string> field_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
