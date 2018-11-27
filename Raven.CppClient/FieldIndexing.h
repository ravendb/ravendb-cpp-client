#pragma once

namespace ravendb::client::documents::indexes
{
	enum class FieldIndexing
	{
		No,
		Search,
		Exact,
		Default
	};
}
