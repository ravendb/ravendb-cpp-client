#pragma once
#include "QueryCommand.h"
#include "FieldsToFetchToken.h"
#include "InMemoryDocumentSessionOperations.h"
#include "SimpleStopWatch.h"

namespace ravendb::client::documents::session::operations
{
	class QueryOperation
	{
	private:
		const std::weak_ptr<InMemoryDocumentSessionOperations> _session;
		const std::optional<std::string> _index_name;
		const queries::IndexQuery _index_query;
		const bool _metadata_only;
		const bool _index_entries_only;
		queries::QueryResult _current_query_result{};
		const std::shared_ptr<tokens::FieldsToFetchToken> _fields_to_fetch;

		impl::SimpleStopWatch _stop_watch{};

	public:
		const bool _no_tracking;

	private:
		void assert_page_size_set() const;

		void start_timing();

	public:
		QueryOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::optional<std::string> index_name,
			queries::IndexQuery index_query, std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch,
			bool disable_entities_tracking, bool metadata_only, bool index_entries_only);

		commands::QueryCommand create_request() const;

		const queries::QueryResult& get_current_query_results() const;

		void set_result(const queries::QueryResult& query_result);

		template<typename T>
		std::vector<std::shared_ptr<T>> complete(const std::optional<DocumentInfo::FromJsonConverter>& from_json = {});

		template<typename T>
		static std::shared_ptr<T> deserialize(const std::string& id, const nlohmann::json& document,
			const nlohmann::json& metadata,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json,
			std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch,
			bool disable_entities_tracking, std::shared_ptr<InMemoryDocumentSessionOperations> session);

		void ensure_is_acceptable_and_save_result(const queries::QueryResult& result);

		static void ensure_is_acceptable(const queries::QueryResult& result, bool wait_for_non_stale_result,
			impl::SimpleStopWatch& stop_watch);

		const queries::IndexQuery& get_index_query() const;
	};

	inline void QueryOperation::start_timing()
	{
		_stop_watch.restart();
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> QueryOperation::complete(const std::optional<DocumentInfo::FromJsonConverter>& from_json)
	{
		auto query_result = _current_query_result.create_snapshot();

		if (!_no_tracking)
		{
			_session.lock()->register_includes(query_result.includes);
		}

		std::vector<std::shared_ptr<T>> list{};

		try
		{
			for (const auto& document : query_result.results)
			{
				auto&& metadata = document.at(constants::documents::metadata::KEY);

				std::string id{};
				if (auto id_json_it = metadata.find(constants::documents::metadata::ID);
					id_json_it != metadata.end() && id_json_it->is_string())
				{
					id = id_json_it->get<std::string>();
				}

				list.push_back(deserialize<T>(id, document, metadata, from_json, _fields_to_fetch, _no_tracking, _session.lock()));
			}
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Unable to read json: " + std::string(e.what()));
		}
		if (_no_tracking)
		{
			_session.lock()->register_missing_includes(query_result.results, query_result.includes, query_result.included_paths);
			//TODO take care of counters
		}
		return list;
	}

	template <typename T>
	std::shared_ptr<T> QueryOperation::deserialize(const std::string& id, const nlohmann::json& document,
		const nlohmann::json& metadata,
		const std::optional<DocumentInfo::FromJsonConverter>& from_json,
		std::shared_ptr<tokens::FieldsToFetchToken> fields_to_fetch,
		bool disable_entities_tracking, std::shared_ptr<InMemoryDocumentSessionOperations> session)
	{
		nlohmann::json the_document = document;

		if (auto projection_it = metadata.find("@projection");
			projection_it == metadata.end() || !projection_it->get<bool>())
		{
			auto from_json_converter = from_json ? *from_json : DocumentInfo::default_from_json<T>;

			return std::static_pointer_cast<T>(session->track_entity(typeid(T), id, document, metadata, disable_entities_tracking,
				from_json_converter, DocumentInfo::default_to_json<T>, DocumentInfo::default_entity_update<T>));
		}

		// we only select a single field
		if (fields_to_fetch && fields_to_fetch->_projections && fields_to_fetch->_projections->size() == 1)
		{
			auto projection_field = std::string_view((*fields_to_fetch->_projections)[0].data(), (*fields_to_fetch->_projections)[0].size());

			if (fields_to_fetch->_source_alias)
			{
				if (projection_field.substr(0, fields_to_fetch->_source_alias->length()) == fields_to_fetch->_source_alias)
				{
					// remove source-alias from projection name
					projection_field.remove_prefix(fields_to_fetch->_source_alias->length());
				}
				if (projection_field.front() == '\'')
				{
					projection_field.remove_prefix(1);
				}
			}
			if constexpr (std::is_same_v<std::string, T> || std::is_fundamental_v<T>)//enum is treated as user type since there is no implicit conversion
			{
				if (auto json_it = document.find(projection_field);
					json_it != document.end() && json_it->is_primitive())
				{
					if (json_it->is_null())
					{
						if constexpr (std::is_default_constructible_v<T>)
						{
							return std::make_shared<T>();
						}
						else
						{
							return std::shared_ptr<T>();
						}
					}
					else
					{
						return std::static_pointer_cast<T>(from_json ? (*from_json)(*json_it) : DocumentInfo::default_from_json<T>(*json_it));
					}
				}
			}
			if (auto json_it = document.find(projection_field);
				json_it != document.end())
			{
				if (json_it->is_null())
				{
					if constexpr (std::is_default_constructible_v<T>)
					{
						return std::make_shared<T>();
					}
					else
					{
						return std::shared_ptr<T>();
					}
				}
				if (fields_to_fetch->_fields_to_fetch[0] == (*fields_to_fetch->_projections)[0] &&
					json_it->is_object())
				{
					the_document = *json_it;
				}

			}
		}

		auto result = from_json ? (*from_json)(the_document) : DocumentInfo::default_from_json<T>(the_document);

		if (!id.empty())
		{
			// we need to make an additional check, since it is possible that a value was explicitly stated
			// for the identity property, in which case we don't want to override it.

			if (! session->get_generate_entity_id_on_the_client().try_get_id_from_instance(typeid(T), result))
			{
				session->get_generate_entity_id_on_the_client().try_set_identity(typeid(T), result, id);
			}
		}

		return std::static_pointer_cast<T>(result);
	}

}
