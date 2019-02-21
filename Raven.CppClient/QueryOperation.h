#pragma once
#include "QueryCommand.h"
#include "FieldsToFetchToken.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::session::operations
{
	class QueryOperation
	{
	private:
		const std::reference_wrapper<InMemoryDocumentSessionOperations> _session;
		const std::optional<std::string> _index_name;
		const queries::IndexQuery _index_query;
		const bool _metadata_only;
		const bool _index_entries_only;
		queries::QueryResult _current_query_result{};
		const std::optional<tokens::FieldsToFetchToken> _fields_to_fetch;
		std::chrono::time_point<std::chrono::steady_clock> _start_time_for_stopwatch;

	public:
		const bool _no_tracking;

	private:
		void assert_page_size_set() const;

		void start_timing();

	public:
		QueryOperation(InMemoryDocumentSessionOperations& session, std::optional<std::string> index_name,
			queries::IndexQuery index_query, std::optional<tokens::FieldsToFetchToken> fields_to_fetch,
			bool disable_entities_tracking, bool metadata_only, bool index_entries_only);

		commands::QueryCommand create_request() const;

		const QueryResult& get_current_query_results() const;

		void set_result(const QueryResult& query_result);

		template<typename T>
		std::vector<std::shared_ptr<T>> complete();

		template<typename T>
		static std::shared_ptr<T> deserialize(const std::string& id, const nlohmann::json& document,
			const nlohmann::json& metadata, const std::optional<tokens::FieldsToFetchToken>& fields_to_fetch,
			bool disable_entities_tracking, InMemoryDocumentSessionOperations& session);

		void ensure_is_acceptable_and_save_result(const QueryResult& result);

		static void ensure_is_acceptable(const QueryResult& result, bool wait_for_non_stale_result,
			const std::chrono::time_point<std::chrono::steady_clock>& start_time_for_stopwatch);

		const IndexQuery& get_index_query() const;
	};

	inline void QueryOperation::start_timing()
	{
		_start_time_for_stopwatch = std::chrono::steady_clock::now();
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> QueryOperation::complete()
	{
		auto query_result = _current_query_result.create_snapshot();

		if (!_no_tracking)
		{
			_session.get().register_includes(query_result.includes);
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

				list.push_back(deserialize<T>(id, document, metadata, _fields_to_fetch, _no_tracking, _session.get()));
			}
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Unable to read json: " + std::string(e.what()));
		}
		if (_no_tracking)
		{
			_session.get().register_missing_includes(query_result.results, query_result.includes, query_result.included_paths);
			//TODO take care of counters
		}
		return list;
	}

	template <typename T>
	std::shared_ptr<T> QueryOperation::deserialize(const std::string& id, const nlohmann::json& document,
		const nlohmann::json& metadata, const std::optional<tokens::FieldsToFetchToken>& fields_to_fetch,
		bool disable_entities_tracking, InMemoryDocumentSessionOperations& session)
	{
		nlohmann::json the_document = document;

		if (auto projection_it = metadata.find("@projection");
			projection_it == metadata.end() || !projection_it->get<bool>())
		{
			return std::static_pointer_cast<T>(session.track_entity(id, document, metadata, disable_entities_tracking,
				DocumentInfo::default_from_json<T>, DocumentInfo::default_to_json<T>));
		}

		if (fields_to_fetch && fields_to_fetch->_projections && fields_to_fetch->_projections->size() == 1)// we only select a single field
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
			if (std::is_same_v<std::string, T> || std::is_fundamental_v<T>)//enum is treated as user type since there is no implicit conversion
			{
				if (auto json_it = document.find(projection_field);
					json_it != document.end() && json_it->is_primitive())
				{
					return json_it->is_null() ? std::make_shared<T>(): std::make_shared<T>(json_it->get<T>());
				}
			}
			if (auto json_it = document.find(projection_field);
				json_it != document.end())
			{
				if (json_it->is_null())
				{
					return std::make_shared<T>();
				}
				if (fields_to_fetch->_fields_to_fetch[0] &&
					fields_to_fetch->_fields_to_fetch[0] == (*fields_to_fetch->_projections)[0] &&
					json_it->is_object())
				{
					the_document = *json_it;
				}

			}
		}

		auto result = std::make_shared<T>(the_document.get<T>());

		if (!id.empty())
		{
			// we need to make an additional check, since it is possible that a value was explicitly stated
			// for the identity property, in which case we don't want to override it.
			//TODO make it
			//Field identityProperty = session.getConventions().getIdentityProperty(clazz);
			//if (identityProperty != null) {
			//	JsonNode value = document.get(identityProperty.getName());

			//	if (value == null) {
			//		session.getGenerateEntityIdOnTheClient().trySetIdentity(result, id);
			//	}
			//}
		}

		return result;
	}

}
