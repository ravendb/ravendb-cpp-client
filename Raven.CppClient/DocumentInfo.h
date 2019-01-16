#pragma once
#include "ConcurrencyCheckMode.h"
#include "Constants.h"
#include "IMetadataDictionary.h"


namespace ravendb::client::documents::session
{
	struct DocumentInfo
	{
		using FromJsonConverter = std::function<std::shared_ptr<void>(const nlohmann::json&)>;
		using ToJsonConverter = std::function<nlohmann::json(std::shared_ptr<void>)>;

		template<typename T>
		inline static const ToJsonConverter default_to_json = [](std::shared_ptr<void> entity) -> nlohmann::json
		{
			return nlohmann::json(*std::static_pointer_cast<T>(entity));
		};

		template<typename T>
		inline static const FromJsonConverter default_from_json = [](const nlohmann::json& json) -> std::shared_ptr<void>
		{
			auto temp = std::make_shared<T>(json.get<T>());//conversion(deserialization)
			return std::static_pointer_cast<void>(temp);
		};

		nlohmann::json document{};
		nlohmann::json metadata{};

		std::shared_ptr<IMetadataDictionary> metadata_instance{};

		std::string id{};
		std::string change_vector{};

		FromJsonConverter from_json_converter;
		ToJsonConverter to_json_converter;
		
		ConcurrencyCheckMode concurrency_check_mode = ConcurrencyCheckMode::AUTO;
		bool ignore_changes = false;

		std::shared_ptr<void> entity{};
		bool new_document = true;
		std::string collection{};

		//TODO std::string entity_type{};
		//std::optional<std::type_info> exact_type;//to be filled by store()

		DocumentInfo() = default;

		//TODO from entity ctor(perhaps template)

		DocumentInfo(nlohmann::json document_)
			: document(std::move(document_))
			, metadata([&]
		{
			auto&& it = document.find(constants::documents::metadata::KEY);
			if (it == document.end() || !it->is_object())
			{
				throw std::runtime_error("Document must have a metadata");
			}
			return *it;
		}())
			, id([&]
		{
			auto&& it = metadata.find(constants::documents::metadata::ID);
			if (it == metadata.end() || !it->is_string())
			{
				throw std::runtime_error("Document must have an id");
			}
			return it->get<std::string>();
		}())
			, change_vector([&]
		{
			auto&& it = metadata.find(constants::documents::metadata::CHANGE_VECTOR);
			if (it == metadata.end() || !it->is_string())
			{
				throw std::runtime_error("Document " + id + " must have a Change Vector");
			}
			return it->get<std::string>();
		}())
		{}

	};
}
