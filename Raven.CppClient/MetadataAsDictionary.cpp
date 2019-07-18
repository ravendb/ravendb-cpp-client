#include "stdafx.h"
#include "MetadataAsDictionary.h"
#include "utils.h"

namespace  ravendb::client::json
{
	MetadataAsDictionary::MetadataAsDictionary() = default;

	MetadataAsDictionary::MetadataAsDictionary(nlohmann::json metadata)
		: _source([&]
	{
		if (!metadata.is_object())
		{
			throw std::invalid_argument("Metadata should be an object");
		}
		return std::move(metadata);
	}())
	{}

	MetadataAsDictionary::MetadataAsDictionary(std::unordered_map<std::string, std::any> metadata)
		: _metadata(std::move(metadata))
	{}

	MetadataAsDictionary::MetadataAsDictionary(nlohmann::json::object_t metadata,
		std::shared_ptr<MetadataAsDictionary> parent, std::string parent_key)
		: MetadataAsDictionary(std::move(metadata))
	{
		if (!parent)
		{
			throw std::invalid_argument("parent cannot be empty");
		}
		if (impl::utils::is_blank(parent_key))
		{
			throw std::invalid_argument("parent_key cannot be blank");
		}

		_parent = parent;
		_parent_key = std::move(parent_key);
	}

	void MetadataAsDictionary::init()
	{
		_dirty = true;
		_metadata.clear();

		update_from_source();

		if (_parent)// mark parent as dirty
		{
			_parent->insert(_parent_key, _weak_this);
		}
	}

	void MetadataAsDictionary::update_from_source()
	{
		for (const auto& field : _source.items())
		{
			_metadata.insert_or_assign(field.key(), convert_value(field.key(), field.value()));
		}
	}

	std::any MetadataAsDictionary::convert_value(const std::string key, const nlohmann::json & value) const
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
			res = create(value, _weak_this.lock(), key);
			break;
		case nlohmann::detail::value_t::array:
		{
			std::vector<std::any> result;
			result.reserve(value.size());
			for (const auto& val : value)
			{
				result.push_back(convert_value(key, val));
			}
			res = std::move(result);
		}
		break;
		default:
			throw std::logic_error("Implement support for numbers and more");
		}
		return res;
	}

	MetadataAsDictionary::~MetadataAsDictionary() = default;

	std::shared_ptr<MetadataAsDictionary> MetadataAsDictionary::create()
	{
		auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary());
		shared->_weak_this = shared;
		return shared;
	}

	std::shared_ptr<MetadataAsDictionary> MetadataAsDictionary::create(nlohmann::json metadata)
	{
		auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(std::move(metadata)));
		shared->_weak_this = shared;
		shared->update_from_source();
		return shared;

	}

	std::shared_ptr<MetadataAsDictionary> MetadataAsDictionary::create(std::unordered_map<std::string, std::any> metadata)
	{
		auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(std::move(metadata)));
		shared->_weak_this = shared;
		shared->update_from_source();
		return shared;
	}

	std::shared_ptr<MetadataAsDictionary> MetadataAsDictionary::create(nlohmann::json::object_t metadata, std::shared_ptr<MetadataAsDictionary> parent, std::string parent_key)
	{
		auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(
			std::move(metadata), parent, std::move(parent_key)));
		shared->_weak_this = shared;
		shared->update_from_source();
		return shared;
	}

	bool MetadataAsDictionary::is_dirty() const
	{
		return _dirty;
	}

	const std::unordered_map<std::string, std::any>& MetadataAsDictionary::get_dictionary() const
	{
		return _metadata;
	}

	bool MetadataAsDictionary::insert(std::string key, std::any value)
	{
		if (_metadata.empty())
		{
			init();
		}
		_dirty = true;

		bool res = false;
		std::tie(std::ignore, res) = _metadata.insert_or_assign(std::move(key), std::move(value));
		return res;
	}

}
