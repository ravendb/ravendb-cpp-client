#include "stdafx.h"
#include "FromToken.h"

namespace ravendb::client::documents::session::tokens
{
	void FromToken::throw_invalid_collection_name() const
	{
		throw std::invalid_argument("Collection name cannot contain a quote, but was: " + *_collection_name);
	}

	const std::optional<std::string>& FromToken::get_collection_name() const
	{
		return _collection_name;
	}

	const std::optional<std::string>& FromToken::get_index_name() const
	{
		return _index_name;
	}

	bool FromToken::is_dynamic() const
	{
		return _dynamic;
	}

	const std::optional<std::string>& FromToken::get_alias() const
	{
		return _alias;
	}

	FromToken::FromToken(std::optional<std::string> index_name, std::optional<std::string> collection_name,
		std::optional<std::string> alias)
		: _index_name(std::move(index_name))
		, _collection_name(std::move(collection_name))
		, _alias(std::move(alias))
		, _dynamic(_collection_name)//using operator bool()
	{}

	void FromToken::write_to(std::ostringstream& writer) const
	{
		if (!_index_name && !_collection_name)
		{
			throw std::runtime_error("Either index_name or collection_name must be specified");
		}

		if(_dynamic)
		{
			writer << "from ";

			if(auto it = std::find_if(_collection_name->begin(), _collection_name->end(),
				[](const int& c)
				{
					return (bool)std::isspace(c);
				});
				it != _collection_name->end())
			{
				if(auto pos = _collection_name->find_first_of('"');
					pos != std::string::npos)
				{
					throw_invalid_collection_name();
				}
				writer << '"' << *_collection_name << '"';
			}else
			{
				write_field(writer, *_collection_name);
			}
		}else
		{
			writer << "from index '" << *_index_name << '\'';
		}
		if(_alias)
		{
			writer << " as " << *_alias;
		}
	}
}
