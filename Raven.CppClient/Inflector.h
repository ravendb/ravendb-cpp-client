#pragma once

namespace ravendb::client::impl
{
	class Inflector
	{
		//TODO implement

	public:

		//TODO make proper implementation
		static std::string pluralize(const std::string word)
		{
			return word + 's';
		}
	};
}
