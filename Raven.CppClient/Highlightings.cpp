#include "stdafx.h"
#include "Highlightings.h"

namespace ravendb::client::documents::queries::highlighting
{
	Highlightings::Highlightings(std::string field_name)
		: _field_name(std::move(field_name))
	{}

	const std::string& Highlightings::get_field_name() const
	{
		return _field_name;
	}

	std::vector<std::string> Highlightings::get_result_indents() const
	{
		std::vector<std::string> res{};
		res.reserve(_highlightings.size());
		for(const auto&[key, val] : _highlightings)
		{
			res.push_back(key);
		}

		return std::move(res);
	}

	const std::vector<std::string>& Highlightings::get_fragments(const std::string& key) const
	{
		static const std::vector<std::string> empty{};

		if(auto it = _highlightings.find(key);
			it == _highlightings.end())
		{
			return empty;
		}
		else
		{
			return  it->second;
		}
	}

	void Highlightings::update(
		const std::optional<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>>&
		highlightings)
	{
		_highlightings.clear();

		if(!highlightings || _highlightings.find(get_field_name()) == _highlightings.end())
		{
			return;
		}

		const auto& result = highlightings->at(get_field_name());
		std::copy(result.cbegin(), result.cend(), std::inserter(_highlightings, _highlightings.end()));
	}
}
