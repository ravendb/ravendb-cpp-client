#pragma once
#include "CompareStringsEqualIgnoreCase.h"

namespace ravendb::client::documents::queries::highlighting
{
	class Highlightings
	{
	private:
		std::unordered_map<std::string, std::vector<std::string>,
			std::hash<std::string>, impl::utils::CompareStringsEqualIgnoreCase> _highlightings{};
		std::string _field_name{};

	public:
		explicit Highlightings(std::string field_name);

		const std::string& get_field_name() const;

		std::vector<std::string> get_result_indents() const;

		const std::vector<std::string>& get_fragments(const std::string& key) const;
		
		void update(const std::optional<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>>&
			highlightings);
	};
}
