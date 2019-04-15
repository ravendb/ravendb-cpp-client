#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class GroupByKeyToken : public QueryToken
	{
	private:
		const std::optional<std::string> _field_name;
		const std::optional<std::string> _projected_name;

		GroupByKeyToken(std::optional<std::string> field_name, std::optional<std::string> projected_name);

	public:
		~GroupByKeyToken() override = default;

		static std::shared_ptr<GroupByKeyToken> create(std::optional<std::string> field_name, std::optional<std::string> projected_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
