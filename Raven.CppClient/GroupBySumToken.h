#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class GroupBySumToken : public QueryToken
	{
	private:
		const std::string _field_name;
		const std::optional<std::string> _projected_name;

		GroupBySumToken(std::string filed_name, std::optional<std::string> projected_name);

	public:
		~GroupBySumToken() override = default;

		static std::shared_ptr<GroupBySumToken> create(std::string filed_name, std::optional<std::string> projected_name);

		void write_to(std::ostringstream& writer) const override;
	};
}
