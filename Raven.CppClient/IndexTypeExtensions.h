#pragma once
#include "IndexType.h"

namespace ravendb::client::documents::indexes
{
	class IndexTypeExtensions
	{
	public:
		IndexTypeExtensions() = delete;

		static bool is_map(IndexType type);

		static bool is_map_reduce(IndexType type);

		static bool is_auto(IndexType type);

		static bool is_state(IndexType type);

		static bool is_java_script(IndexType type);
	};
}
