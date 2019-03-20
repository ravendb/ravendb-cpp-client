#include "stdafx.h"
#include "DocumentConventions.h"
#include "Constants.h"
#include "CompareStringsIgnoreCase.h"
#include "Inflector.h"
#include "GetCppClassName.h"

namespace ravendb::client::documents::conventions
{
	std::shared_mutex DocumentConventions::_ids_helpers_guard{};
	std::unordered_map<std::type_index, EntityIdHelper> DocumentConventions::_id_helpers{};

	std::shared_mutex DocumentConventions::_cached_default_type_collection_names_guard{};
	std::unordered_map<std::type_index, std::string> DocumentConventions::_cached_default_type_collection_names{};

	void DocumentConventions::assert_not_frozen() const
	{
		if(_frozen)
		{
			throw std::runtime_error(
				"Conventions has been frozen after document_store::initialize() and no changes can be applied to them");
		}
	}

	DocumentConventions::DocumentConventions()
		: _identity_part_separator("/")
		, _transform_class_collection_name_to_document_id_prefix([](const std::string& collection_name)->std::string
	{
		return default_transform_collection_name_to_document_id_prefix(collection_name);
	})
		, _find_collection_name([](std::type_index type)->std::string
	{
		return default_get_collection_name(type);
	})
		, _find_cpp_class_name([](std::type_index type)->std::string
	{
		return impl::utils::GetCppClassName()(type);
	})
		, _find_cpp_class([](const std::string& id, const nlohmann::json& doc)->std::optional<std::string>
	{
		if (auto metadata_it = doc.find(constants::documents::metadata::KEY);
			metadata_it != doc.end())
		{
			if (auto cpp_type_it = metadata_it->find(constants::documents::metadata::RAVEN_CPP_TYPE);
				cpp_type_it != metadata_it->end())
			{
				return cpp_type_it->get<std::string>();
			}
		}
		return {};
	})
		, _max_number_of_requests_per_session(30)
		, _read_balance_behaviour(http::ReadBalanceBehavior::NONE)
		, _max_http_cache_size(128 * 1024 * 1024)
	{}

	DocumentConventions::DocumentConventions(const DocumentConventions& other)
		: _frozen{ other._frozen },
		_original_configuration{
		  other._original_configuration ? std::make_unique<operations::configuration::ClientConfiguration>(*other._original_configuration) :
			std::make_unique<operations::configuration::ClientConfiguration>()},
		_save_enums_as_integers{ other._save_enums_as_integers },
		_identity_part_separator{ other._identity_part_separator },
		_disable_topology_updates{ other._disable_topology_updates },
		_transform_class_collection_name_to_document_id_prefix{
			  other._transform_class_collection_name_to_document_id_prefix},
		_document_id_generator{ other._document_id_generator },
		_find_collection_name{ other._find_collection_name },
		_find_cpp_class_name{ other._find_cpp_class_name },
		_find_cpp_class{ other._find_cpp_class },
		_use_optimistic_concurrency{ other._use_optimistic_concurrency },
		_throw_if_query_page_size_is_not_set{ other._throw_if_query_page_size_is_not_set },
		_max_number_of_requests_per_session{ other._max_number_of_requests_per_session },
		_read_balance_behaviour{ other._read_balance_behaviour },
		_max_http_cache_size{ other._max_http_cache_size },
		_use_compression{ other._use_compression }
	{}

	void DocumentConventions::add_entity_id_helper(std::type_index type, EntityIdHelper id_helper)
	{
		auto lock = std::unique_lock(_ids_helpers_guard);
		_id_helpers.insert_or_assign(type, std::move(id_helper));
	}

	std::optional<EntityIdHelper> DocumentConventions::get_entity_id_helper(std::type_index type)
	{
		auto lock = std::shared_lock(_ids_helpers_guard);
		if(auto it = _id_helpers.find(type);
			it != _id_helpers.end())
		{
			return it->second;
		}
		return {};
	}

	bool DocumentConventions::has_explicitly_set_compression_usage() const
	{
		return _use_compression.has_value();
	}

	bool DocumentConventions::is_use_compression() const
	{
		return _use_compression.value_or(true);
	}

	void DocumentConventions::set_use_compression(std::optional<bool> use_compression)
	{
		assert_not_frozen();
		_use_compression = std::move(use_compression);
	}

	http::ReadBalanceBehavior DocumentConventions::get_read_balance_behavior() const
	{
		return _read_balance_behaviour;
	}

	void DocumentConventions::set_read_balance_behavior(http::ReadBalanceBehavior read_balance_behaviour)
	{
		assert_not_frozen();
		_read_balance_behaviour = read_balance_behaviour;
	}

	int32_t DocumentConventions::get_max_http_cache_size() const
	{
		return _max_http_cache_size;
	}

	void DocumentConventions::set_max_http_cache_size(int32_t max_http_cache_size)
	{
		assert_not_frozen();
		_max_http_cache_size = max_http_cache_size;
	}

	int32_t DocumentConventions::get_max_number_of_requests_per_session() const
	{
		return _max_number_of_requests_per_session;
	}

	void DocumentConventions::set_max_number_of_requests_per_session(int32_t max_number_of_requests_per_session)
	{
		assert_not_frozen();
		_max_number_of_requests_per_session = max_number_of_requests_per_session;
	}

	bool DocumentConventions::is_throw_if_query_page_size_is_not_set() const
	{
		return _throw_if_query_page_size_is_not_set;
	}

	void DocumentConventions::set_throw_if_query_page_size_is_not_set(bool throw_if_query_page_size_is_not_set)
	{
		assert_not_frozen();
		_throw_if_query_page_size_is_not_set = throw_if_query_page_size_is_not_set;
	}

	bool DocumentConventions::is_use_optimistic_concurrency() const
	{
		return _use_optimistic_concurrency;
	}

	void DocumentConventions::set_use_optimistic_concurrency(bool use_optimistic_concurrency)
	{
		assert_not_frozen();
		_use_optimistic_concurrency = use_optimistic_concurrency;
	}

	std::function<std::optional<std::string>(const std::string&, const nlohmann::json&)> 
		DocumentConventions::get_find_cpp_class() const
	{
		return _find_cpp_class;
	}

	void DocumentConventions::set_find_cpp_class(
		std::function<std::optional<std::string>(const std::string&, const nlohmann::json&)> find_cpp_class)
	{
		assert_not_frozen();
		_find_cpp_class = find_cpp_class;
	}

	std::function<std::string(std::type_index)> DocumentConventions::get_find_cpp_class_name() const
	{
		return _find_cpp_class_name;
	}

	void DocumentConventions::set_find_cpp_class_name(std::function<std::string(std::type_index)> find_cpp_class_name)
	{
		assert_not_frozen();
		_find_cpp_class_name = find_cpp_class_name;
	}

	std::function<std::optional<std::string>(std::type_index)> DocumentConventions::get_find_collection_name() const
	{
		return _find_collection_name;
	}

	void DocumentConventions::set_find_collection_name(std::function<std::optional<std::string>(std::type_index)> find_collection_name)
	{
		assert_not_frozen();
		_find_collection_name = find_collection_name;
	}


	std::function<std::string(const std::string&, std::shared_ptr<void>, std::type_index)> 
		DocumentConventions::get_document_id_generator() const
	{
		return _document_id_generator;
	}

	void DocumentConventions::set_document_id_generator(
		std::function<std::string(const std::string&, std::shared_ptr<void>, std::type_index)> document_id_generator)
	{
		assert_not_frozen();
		_document_id_generator = document_id_generator;
	}

	std::function<std::string(const std::string&)> DocumentConventions::get_transform_class_collection_name_to_document_id_prefix() const
	{
		return _transform_class_collection_name_to_document_id_prefix;
	}

	void DocumentConventions::set_transform_class_collection_name_to_document_id_prefix(
		std::function<std::string(const std::string&)> transform_class_collection_name_to_document_id_prefix)
	{
		assert_not_frozen();
		_transform_class_collection_name_to_document_id_prefix = transform_class_collection_name_to_document_id_prefix;
	}

	bool DocumentConventions::is_disable_topology_updates() const
	{
		return _disable_topology_updates;
	}

	void DocumentConventions::set_disable_topology_updates(bool disable_topology_updates)
	{
		assert_not_frozen();
		_disable_topology_updates = disable_topology_updates;
	}

	const std::string& DocumentConventions::get_identity_part_separator() const
	{
		return _identity_part_separator;
	}

	void DocumentConventions::set_identity_part_separator(std::string identity_part_separator)
	{
		assert_not_frozen();
		_identity_part_separator = std::move(identity_part_separator);
	}

	bool DocumentConventions::is_save_enums_as_integers() const
	{
		return _save_enums_as_integers;
	}

	void DocumentConventions::set_save_enums_as_integers(bool save_enums_as_integers)
	{
		assert_not_frozen();
		_save_enums_as_integers = save_enums_as_integers;
	}

	std::shared_ptr<DocumentConventions> DocumentConventions::default_conventions()
	{
		auto conventions = std::make_shared<DocumentConventions>();
		conventions->freeze();
		return conventions;
	}

	std::string DocumentConventions::generate_document_id(const std::string database_name,
		std::type_index type, std::shared_ptr<void> entity)
	{
		if (auto it = _list_of_registered_id_conventions.find(type);
			it != _list_of_registered_id_conventions.end())
		{
			return (it->second)(database_name, entity);
		}

		return _document_id_generator(database_name, entity, type);
	}

	std::optional<std::string> DocumentConventions::get_cpp_class(const std::string& id, const nlohmann::json& document)
	{
		return _find_cpp_class(id, document);
	}

	std::string DocumentConventions::get_cpp_class_name(std::type_index entity_type)
	{
		return _find_cpp_class_name(entity_type);
	}

	void DocumentConventions::update_from(const operations::configuration::ClientConfiguration& configuration)
	{
		static std::mutex m{};

		auto lock = std::lock_guard(m);

		if(configuration.disabled && !_original_configuration)
		{
			return;// nothing to do
		}

		if(configuration.disabled && !_original_configuration)// need to revert to original values
		{
			if (_original_configuration->max_number_of_requests_per_session)
			{
				_max_number_of_requests_per_session = _original_configuration->max_number_of_requests_per_session.value();
			}
			if (_original_configuration->read_balance_behaviour)
			{
				_read_balance_behaviour = _original_configuration->read_balance_behaviour.value();
			}

			_original_configuration.reset();
			return;
		}

		if(!_original_configuration)
		{
			_original_configuration = std::make_unique<operations::configuration::ClientConfiguration>();
			_original_configuration->etag = -1;
			_original_configuration->max_number_of_requests_per_session = _max_number_of_requests_per_session;
			_original_configuration->read_balance_behaviour = _read_balance_behaviour;
		}

		if(configuration.max_number_of_requests_per_session)
		{
			_max_number_of_requests_per_session = configuration.max_number_of_requests_per_session.value();
		}else if(_original_configuration->max_number_of_requests_per_session)
		{
			_max_number_of_requests_per_session = _original_configuration->max_number_of_requests_per_session.value();
		}

		if(configuration.read_balance_behaviour)
		{
			_read_balance_behaviour = configuration.read_balance_behaviour.value();
		}
		else if(_original_configuration->read_balance_behaviour)
		{
			_read_balance_behaviour = _original_configuration->read_balance_behaviour.value();
		}
	}

	std::string DocumentConventions::default_transform_collection_name_to_document_id_prefix(
		const std::string& collection_name)
	{
		const int64_t upper_count = std::accumulate(collection_name.cbegin(), collection_name.cend(), 0,
			[](const int64_t& count, std::string::value_type c)
		{
			return count + bool(std::isupper(c)) ? 1 : 0;
		});

		if(upper_count <= 1)
		{
			return impl::utils::CompareStringsIgnoreCase::to_lower_str(collection_name);
		}

		// multiple capital letters, so probably something that we want to preserve caps on.
		return collection_name;
	}

	void DocumentConventions::freeze()
	{
		_frozen = true;
	}

	std::string DocumentConventions::default_get_collection_name(std::type_index type)
	{
		{
			auto guard = std::shared_lock(_cached_default_type_collection_names_guard);
			if (auto it = _cached_default_type_collection_names.find(type);
				it != _cached_default_type_collection_names.end())
			{
				return it->second;
			}
		}

		std::string simple_class_name = impl::utils::GetCppClassName::get_simple_class_name(type);
		auto result = impl::Inflector::pluralize(simple_class_name);

		{
			auto guard = std::unique_lock(_cached_default_type_collection_names_guard);
			if (auto it = _cached_default_type_collection_names.find(type);
				it != _cached_default_type_collection_names.end())
			{
				return it->second;
			}
			_cached_default_type_collection_names.insert_or_assign(type, result);
		}

		return result;
	}

	std::string DocumentConventions::get_collection_name(std::type_index type) const
	{
		if(auto res = _find_collection_name(type);
			res.has_value())
		{
			return res.value();
		}
		return default_get_collection_name(type);
	}

}
