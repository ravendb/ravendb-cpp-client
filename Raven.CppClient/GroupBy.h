#pragma once
#include "GroupByMethod.h"

namespace ravendb::client::documents::queries
{
	class GroupBy
	{
	private:
		std::string _field{};
		GroupByMethod _method{};

		GroupBy() = default;

	public:
		const std::string& get_field() const;
		GroupByMethod get_method() const;

		static GroupBy field(std::string field_name);
		static GroupBy array(std::string field_name);
	};
}
