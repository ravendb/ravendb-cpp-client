#pragma once
#include <typeindex>
#include "ClientConfiguration.h"
#include "EntityIdHelper.h"
#include <shared_mutex>

namespace ravendb::client::documents::conventions
{
	//TODO consider using the BUILDER pattern 
	class DocumentConventions
	{
	private:
		static std::shared_mutex _ids_helpers_guard;
		static std::unordered_map<std::type_index, EntityIdHelper> _id_helpers;

		static std::unordered_map<std::type_index, std::string> _cached_default_type_collection_names;

		//TODO
		//std::vector<std::pair<std::type_index, IValueForQueryConverter<Object>>> _list_of_query_value_to_object_converters{};

		std::unordered_map<std::type_index, std::function<std::string(std::string, std::shared_ptr<void>)>>
			_list_of_registered_id_conventions{};

		bool _frozen = false;
		std::unique_ptr<operations::configuration::ClientConfiguration> _original_configuration{};
		bool _save_enums_as_integers = false;
		std::string _identity_part_separator{};
		bool _disable_topology_updates = false;

		std::function<std::string(const std::string&)> _transform_class_collection_name_to_document_id_prefix{};
		std::function<std::string(const std::string&, std::shared_ptr<void>, std::type_index)> _document_id_generator{};

		std::function<std::optional<std::string>(std::type_index)> _find_collection_name{};

		std::function<std::string(std::type_index)> _find_cpp_class_name{};
		std::function<std::optional<std::string>(const std::string&, const nlohmann::json&)> _find_cpp_class{};

		bool _use_optimistic_concurrency = true;
		bool _throw_if_query_page_size_is_not_set = false;
		int32_t _max_number_of_requests_per_session{};

		http::ReadBalanceBehavior _read_balance_behaviour{};
		int32_t _max_http_cache_size{};
		std::optional<bool> _use_compression{};


		void assert_not_frozen() const;

	public:
		~DocumentConventions() = default;

		static std::shared_ptr<DocumentConventions> default_conventions();

		DocumentConventions();

		DocumentConventions(const DocumentConventions& other);

		//changed to the new one if already exists
		static void add_entity_id_helper(std::type_index type, EntityIdHelper id_helper);

		static std::optional<EntityIdHelper> get_entity_id_helper(std::type_index type);

		bool has_explicitly_set_compression_usage() const;

		bool is_use_compression() const;

		void set_use_compression(std::optional<bool> use_compression);

		http::ReadBalanceBehavior get_read_balance_behavior() const;

		void set_read_balance_behavior(http::ReadBalanceBehavior read_balance_behaviour);

		int32_t get_max_http_cache_size() const;

		void set_max_http_cache_size(int32_t max_http_cache_size);

		int32_t get_max_number_of_requests_per_session() const;

		void set_max_number_of_requests_per_session(int32_t max_number_of_requests_per_session);

		bool is_throw_if_query_page_size_is_not_set() const;

		void set_throw_if_query_page_size_is_not_set(bool throw_if_query_page_size_is_not_set);

		bool is_use_optimistic_concurrency() const;

		void set_use_optimistic_concurrency(bool use_optimistic_concurrency);

		std::function<std::optional<std::string>(const std::string&, const nlohmann::json&)> get_find_cpp_class() const;

		void set_find_cpp_class(std::function<std::optional<std::string>(const std::string&, const nlohmann::json&)>
			find_cpp_class);

		std::function<std::string(std::type_index)> get_find_cpp_class_name() const;

		void set_find_cpp_class_name(std::function<std::string(std::type_index)> find_cpp_class_name);

		std::function<std::optional<std::string>(std::type_index)> get_find_collection_name() const;

		void set_find_collection_name(std::function<std::optional<std::string>(std::type_index)> find_collection_name);

		std::function<std::string(const std::string&, std::shared_ptr<void>, std::type_index)> get_document_id_generator() const;

		void set_document_id_generator(std::function<std::string(const std::string&, std::shared_ptr<void>, std::type_index)>
			document_id_generator);

		std::function<std::string(const std::string&)> get_transform_class_collection_name_to_document_id_prefix() const;

		void set_transform_class_collection_name_to_document_id_prefix(std::function<std::string(const std::string&)>
			transform_class_collection_name_to_document_id_prefix);

		bool is_disable_topology_updates() const;

		void set_disable_topology_updates(bool disable_topology_updates);

		const std::string& get_identity_part_separator() const;

		void set_identity_part_separator(std::string identity_part_separator);

		bool is_save_enums_as_integers() const;

		void set_save_enums_as_integers(bool save_enums_as_integers);

		static std::string default_get_collection_name(std::type_index type);

		std::string get_collection_name(std::type_index type) const;

		std::string generate_document_id(const std::string database_name,
			std::type_index type, std::shared_ptr<void> entity);

		std::optional<std::string> get_cpp_class(const std::string& id, const nlohmann::json& document);

		std::string get_cpp_class_name(std::type_index entity_type);

		void update_from(const operations::configuration::ClientConfiguration& configuration);

		static std::string default_transform_collection_name_to_document_id_prefix(const std::string& collection_name);

		void freeze();
	};
}
