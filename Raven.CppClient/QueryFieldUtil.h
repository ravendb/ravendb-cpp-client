#pragma once

namespace ravendb::client::documents::queries
{
	struct QueryFieldUtil
	{
		~QueryFieldUtil() = delete;

		static std::string escape_if_necessary(std::string name);
	};
}
