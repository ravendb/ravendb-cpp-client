#pragma once

namespace ravendb::client::documents::session
{
	template<typename TValue>
	struct WhereParams
	{
		std::string field_name{};
		const TValue* value{};
		bool allow_wildcards{ false };
		bool nested_path{ false };
		bool exact{};
	};
}
