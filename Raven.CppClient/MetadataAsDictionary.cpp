#include "stdafx.h"
#include "MetadataAsDictionary.h"
#include "MetadataDictionaryArray.h"

namespace  ravendb::client::json
{
	void MetadataAsDictionary::update_from_source(const nlohmann::json& source)
	{
		for (const auto& [key,value] : source.items())
		{
			_metadata.insert_or_assign(key, convert_value(value));
		}
	}

	std::any MetadataAsDictionary::convert_value(const nlohmann::json& value)
	{
		std::any res{};

		switch (value.type())
		{
		case nlohmann::detail::value_t::null:
			break;
		case nlohmann::detail::value_t::number_integer:
			res = value.get<int64_t>();
			break;
		case nlohmann::detail::value_t::number_unsigned:
			res = value.get<uint64_t>();
			break;
		case nlohmann::detail::value_t::number_float:
			res = value.get<double>();
			break;
		case nlohmann::detail::value_t::boolean:
			res = value.get<bool>();
			break;
		case nlohmann::detail::value_t::string:
			res = value.get<std::string>();
			break;
		case nlohmann::detail::value_t::object:
			res = MetadataAsDictionary(value);
			break;
		case nlohmann::detail::value_t::array:
		{
			MetadataDictionaryArray result{};
			for (const auto& val : value)
			{
				result.push_back(convert_value(val));
			}
			res = std::move(result);
		}
			break;
		default:
			throw std::logic_error("Implement support for numbers and more");
		}
		return res;
	}

	void MetadataAsDictionary::set_dirty()
	{
		_dirty = true;
	}

	MetadataAsDictionary::MetadataAsDictionary(const nlohmann::json& metadata)
	{
		
		if (metadata.is_null())
		{
			return;
		}

		if (!metadata.is_object())
		{
			throw std::invalid_argument("Metadata should be an object");
		}

		update_from_source(metadata);
	}

	bool MetadataAsDictionary::is_dirty() const
	{
		return _dirty;
	}

	const MetadataAsDictionary& MetadataAsDictionary::get_as_dict(const std::string& key) const
	{
		return this->get_as<MetadataAsDictionary>(key);
	}

	MetadataAsDictionary& MetadataAsDictionary::get_as_dict(const std::string& key)
	{
		return this->get_as<MetadataAsDictionary>(key);
	}

	const MetadataDictionaryArray& MetadataAsDictionary::get_as_array(const std::string& key) const
	{
		return this->get_as<MetadataDictionaryArray>(key);
	}

	MetadataDictionaryArray& MetadataAsDictionary::get_as_array(const std::string& key)
	{
		return this->get_as<MetadataDictionaryArray>(key);
	}

	bool MetadataAsDictionary::empty() const
	{
		return _metadata.empty();
	}

	size_t MetadataAsDictionary::size() const
	{
		return _metadata.size();
	}

	void MetadataAsDictionary::clear()
	{
		set_dirty();

		_metadata.clear();
	}

	bool MetadataAsDictionary::contains(const std::string& key) const
	{
		return _metadata.count(key) > 0;
	}

	bool MetadataAsDictionary::erase(const std::string& key)
	{
		set_dirty();
		return _metadata.erase(key) > 0;
	}

	MetadataAsDictionary::iterator MetadataAsDictionary::begin()
	{
		return _metadata.begin();
	}

	MetadataAsDictionary::const_iterator MetadataAsDictionary::begin() const
	{
		return cbegin();
	}

	MetadataAsDictionary::const_iterator MetadataAsDictionary::cbegin() const
	{
		return _metadata.cbegin();
	}

	MetadataAsDictionary::iterator MetadataAsDictionary::end()
	{
		return _metadata.end();
	}

	MetadataAsDictionary::const_iterator MetadataAsDictionary::end() const
	{
		return cend();
	}

	MetadataAsDictionary::const_iterator MetadataAsDictionary::cend() const
	{
		return _metadata.cend();
	}

	const MetadataAsDictionary& MetadataAsDictionary::get_dict(const std::any& value)
	{
		return std::any_cast<const MetadataAsDictionary&>(value);
	}

	MetadataAsDictionary& MetadataAsDictionary::get_dict(std::any& value)
	{
		return std::any_cast<MetadataAsDictionary&>(value);
	}

	const MetadataAsDictionary* MetadataAsDictionary::get_dict(const std::any* value)
	{
		return std::any_cast<MetadataAsDictionary>(value);
	}

	MetadataAsDictionary* MetadataAsDictionary::get_dict(std::any* value)
	{
		return std::any_cast<MetadataAsDictionary>(value);
	}
}
