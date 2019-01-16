#pragma once
#include "IMetadataDictionary.h"
#include "utils.h"

namespace  ravendb::client::json
{
	class MetadataAsDictionary : public documents::session::IMetadataDictionary
	{
	private:
		std::weak_ptr<MetadataAsDictionary> shared_this{};

		std::shared_ptr<MetadataAsDictionary> _parent{};
		std::string _parent_key{};
		std::unordered_map<std::string, std::any> _metadata{};
		nlohmann::json _source{};

		bool _dirty = false;

		MetadataAsDictionary() = default;

		explicit MetadataAsDictionary(nlohmann::json metadata)
			: _source([&]
		{
			if(!metadata.is_object())
			{
				throw std::invalid_argument("Metadata should be object");
			}
			return std::move(metadata);
		}())
		{}

		explicit MetadataAsDictionary(std::unordered_map<std::string, std::any> metadata)
			: _metadata(std::move(metadata))
			, _source()
		{}

		MetadataAsDictionary(nlohmann::json::object_t metadata, std::shared_ptr<MetadataAsDictionary> parent,
			std::string parent_key)
			: MetadataAsDictionary(std::move(metadata))
		{
			if (!parent)
			{
				throw std::invalid_argument("parent cannot be null");
			}
			if (impl::utils::is_blank(parent_key))
			{
				throw std::invalid_argument("parent_key cannot be blank");
			}

			_parent = parent;
			_parent_key = std::move(parent_key);
		}

		void init()
		{
			_dirty = true;
			_metadata.clear();

			for (const auto& field : _source.items())
			{
				_metadata.insert({field.key(), convert_value(field.key(), field.value())});
			}

			if(_parent)// mark parent as dirty
			{
				_parent->insert(_parent_key, shared_this);
			}
		}

		std::any convert_value(const std::string key, const nlohmann::json& value) const
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
				res = create(value, shared_this.lock(), key);
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
		
	public:
		~MetadataAsDictionary() override = default;

		static std::shared_ptr<MetadataAsDictionary> create()
		{
			auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary());
			shared->shared_this = shared;
			return shared;
		}
		static std::shared_ptr<MetadataAsDictionary> create(nlohmann::json metadata)
		{
			auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(std::move(metadata)));
			shared->shared_this = shared;
			return shared;

		}
		static std::shared_ptr<MetadataAsDictionary> create(std::unordered_map<std::string, std::any> metadata)
		{
			auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(std::move(metadata)));
			shared->shared_this = shared;
			return shared;
		}
		static std::shared_ptr<MetadataAsDictionary> create(nlohmann::json::object_t metadata,
			std::shared_ptr<MetadataAsDictionary> parent, std::string parent_key)
		{
			auto shared = std::shared_ptr<MetadataAsDictionary>(new MetadataAsDictionary(
				std::move(metadata), parent, std::move(parent_key)));
			shared->shared_this = shared;
			return shared;
		}

		bool is_dirty() const override
		{
			return _dirty;
		}
		const std::unordered_map<std::string, std::any>& get_dictionary() const override
		{
			return _metadata;
		}

		bool insert(std::string key, std::any value) override
		{
			if(_metadata.empty())
			{
				init();
			}
			_dirty = true;

			bool res = false;
			std::tie(std::ignore, res) = _metadata.insert_or_assign(std::move(key), std::move(value));
			return res;
		}
	};
}

