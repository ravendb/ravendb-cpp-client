#include "stdafx.h"
#include "CounterIncludesToken.h"

namespace ravendb::client::documents::session::tokens
{
	CounterIncludesToken::CounterIncludesToken(std::string source_path, std::optional<std::string> parameter_name, bool all)
		: _source_path(std::move(source_path))
		, _parameter_name(std::move(parameter_name))
		, _all(all)
	{}

	std::shared_ptr<CounterIncludesToken> CounterIncludesToken::create(std::string source_path,
		std::optional<std::string> parameter_name)
	{
		return std::shared_ptr<CounterIncludesToken>(new CounterIncludesToken(std::move(source_path), std::move(parameter_name), false));
	}

	std::shared_ptr<CounterIncludesToken> CounterIncludesToken::all(std::string source_path)
	{
		return std::shared_ptr<CounterIncludesToken>(new CounterIncludesToken(std::move(source_path), {}, true));
	}

	void CounterIncludesToken::add_alias_to_path(const std::string& alias)
	{
		if(_source_path.empty())
		{
			_source_path = alias;
		}
		else
		{
			std::ostringstream path{};
			path << alias << "." << _source_path;
			_source_path = path.str();
		}
	}

	void CounterIncludesToken::write_to(std::ostringstream& writer) const
	{
		writer << "counters(";

		if(!_source_path.empty())
		{
			writer << _source_path;
			if(!_all)
			{
				writer << ", ";
			}
		}

		if(!_all)
		{
			writer << "$" << (_parameter_name ? *_parameter_name : "");
		}
		writer << ")";
	}
}
