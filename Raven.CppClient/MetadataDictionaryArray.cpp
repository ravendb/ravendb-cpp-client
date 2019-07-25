#include "stdafx.h"
#include "MetadataDictionaryArray.h"
#include "MetadataAsDictionary.h"

namespace ravendb::client::json
{
	const MetadataDictionaryArray& MetadataDictionaryArray::get_array(const std::any& value)
	{
		return std::any_cast<const MetadataDictionaryArray&>(value);
	}

	MetadataDictionaryArray& MetadataDictionaryArray::get_array(std::any& value)
	{
		return std::any_cast<MetadataDictionaryArray&>(value);
	}

	const MetadataDictionaryArray* MetadataDictionaryArray::get_array(const std::any* value)
	{
		return std::any_cast<MetadataDictionaryArray>(value);
	}

	MetadataDictionaryArray* MetadataDictionaryArray::get_array(std::any* value)
	{
		return std::any_cast<MetadataDictionaryArray>(value);
	}

	const MetadataAsDictionary& MetadataDictionaryArray::get_as_dict(size_t index) const
	{
		return this->get_as<MetadataAsDictionary>(index);
	}

	MetadataAsDictionary& MetadataDictionaryArray::get_as_dict(size_t index)
	{
		return this->get_as<MetadataAsDictionary>(index);
	}

	const MetadataDictionaryArray& MetadataDictionaryArray::get_as_array(size_t index) const
	{
		return this->get_as<MetadataDictionaryArray>(index);
	}

	MetadataDictionaryArray& MetadataDictionaryArray::get_as_array(size_t index)
	{
		return this->get_as<MetadataDictionaryArray>(index);
	}

	bool MetadataDictionaryArray::empty() const
	{
		return _metadata.empty();
	}

	size_t MetadataDictionaryArray::size() const
	{
		return _metadata.size();
	}

	void MetadataDictionaryArray::reserve(size_t new_capacity)
	{
		_metadata.reserve(new_capacity);
	}

	void MetadataDictionaryArray::clear()
	{
		_metadata.clear();
	}

	void MetadataDictionaryArray::erase(size_t index)
	{
		_metadata.erase(_metadata.begin() + index);
	}

	MetadataDictionaryArray::iterator MetadataDictionaryArray::begin()
	{
		return _metadata.begin();
	}

	MetadataDictionaryArray::const_iterator MetadataDictionaryArray::begin() const
	{
		return cbegin();
	}

	MetadataDictionaryArray::const_iterator MetadataDictionaryArray::cbegin() const
	{
		return _metadata.cbegin();
	}

	MetadataDictionaryArray::iterator MetadataDictionaryArray::end()
	{
		return _metadata.end();
	}

	MetadataDictionaryArray::const_iterator MetadataDictionaryArray::end() const
	{
		return cend();
	}

	MetadataDictionaryArray::const_iterator MetadataDictionaryArray::cend() const
	{
		return _metadata.cend();
	}
}
