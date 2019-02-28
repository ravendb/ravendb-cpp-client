#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class FromToken : public QueryToken
	{
	private:
		const std::optional<std::string> _index_name;
		const std::optional<std::string> _collection_name;
		const std::optional<std::string> _alias;
		const bool _dynamic;

		void throw_invalid_collection_name() const;

	public:
		~FromToken() override = default;

		const std::optional<std::string>& get_collection_name() const;

		const std::optional<std::string>& get_index_name() const;

		bool is_dynamic() const;

		const std::optional<std::string>& get_alias() const;

		FromToken(std::optional<std::string> index_name,
			std::optional<std::string> collection_name,
			std::optional<std::string> alias = {});

		void write_to(std::ostringstream& oss) const override;
	};
}
