#pragma once
#include "QueryToken.h"
#include "GroupByMethod.h"

namespace ravendb::client::documents::session::tokens
{
	class GroupByToken : public QueryToken
	{
	private:
		const std::string _filed_name;
		const queries::GroupByMethod _method;

		GroupByToken(std::string field_name, queries::GroupByMethod method);

	public:
		~GroupByToken() override = default;

		static std::shared_ptr<GroupByToken> create(std::string field_name,
			queries::GroupByMethod method = queries::GroupByMethod::NONE);

		void write_to(std::ostringstream& writer) const override;
	};
}
