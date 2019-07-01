#pragma once
#include "IMetadataDictionary.h"
#include <json.hpp>

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

		MetadataAsDictionary();

		explicit MetadataAsDictionary(nlohmann::json metadata);

		explicit MetadataAsDictionary(std::unordered_map<std::string, std::any> metadata);

		MetadataAsDictionary(nlohmann::json::object_t metadata, std::shared_ptr<MetadataAsDictionary> parent,
			std::string parent_key);

		void init();

		void update_from_source();

		std::any convert_value(const std::string key, const nlohmann::json& value) const;
		
	public:
		~MetadataAsDictionary() override;

		static std::shared_ptr<MetadataAsDictionary> create();

		static std::shared_ptr<MetadataAsDictionary> create(nlohmann::json metadata);

		static std::shared_ptr<MetadataAsDictionary> create(std::unordered_map<std::string, std::any> metadata);

		static std::shared_ptr<MetadataAsDictionary> create(nlohmann::json::object_t metadata,
			std::shared_ptr<MetadataAsDictionary> parent, std::string parent_key);

		bool is_dirty() const override;

		const std::unordered_map<std::string, std::any>& get_dictionary() const override;

		bool insert(std::string key, std::any value) override;
	};
}

