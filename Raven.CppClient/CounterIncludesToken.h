#pragma once
#include "QueryToken.h"

namespace ravendb::client::documents::session::tokens
{
	class CounterIncludesToken : public QueryToken
	{
	private:
		std::string _source_path;
		const std::optional<std::string> _parameter_name;
		const bool _all;

		CounterIncludesToken(std::string source_path, std::optional<std::string> parameter_name, bool all);

	public:
		~CounterIncludesToken() override = default;
		
		static std::shared_ptr<CounterIncludesToken> create(std::string source_path, std::optional<std::string> parameter_name);

		static std::shared_ptr<CounterIncludesToken> all(std::string source_path);

		void add_alias_to_path(const std::string& alias);

		void write_to(std::ostringstream& writer) const override;
	};
}
