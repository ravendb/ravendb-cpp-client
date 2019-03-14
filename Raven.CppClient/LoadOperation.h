#pragma once
#include "InMemoryDocumentSessionOperations.h"
#include "GetDocumentsResult.h"
#include "GetDocumentsCommand.h"
#include "DocumentsByIdsMap.h"

namespace ravendb::client::documents::session::operations
{
	class LoadOperation
	{
	private:
		const std::shared_ptr<InMemoryDocumentSessionOperations> _session;

		std::vector<std::string> _ids{};

		std::vector<std::string> _includes{};

		std::vector<std::string> _ids_to_check_on_server{};

		std::optional<commands::GetDocumentsResult> _current_load_results{};

	public:
		LoadOperation(std::shared_ptr<InMemoryDocumentSessionOperations> session)
			: _session(session)
		{}

		std::unique_ptr<RavenCommand<commands::GetDocumentsResult>> create_request() const
		{
			if(_ids_to_check_on_server.empty())
			{
				return {};
			}

			if(_session->check_if_already_included(_ids, _includes))
			{
				return nullptr;
			}

			_session->increment_request_count();

			//TODO take care of counters

			return std::make_unique<commands::GetDocumentsCommand>(_ids_to_check_on_server, _includes, false);
		}

		LoadOperation& with_includes(std::vector<std::string> includes)
		{
			_includes = std::move(includes);

			return *this;
		}

		LoadOperation& by_id(const std::string& id)
		{
			if(_ids.empty())
			{
				_ids.push_back(id);
			}
			if(_session->is_loaded_or_deleted(id))
			{
				return *this;
			}

			_ids_to_check_on_server.push_back(id);
			return *this;
		}

		LoadOperation& by_ids(const std::vector<std::reference_wrapper<const std::string>>& ids)
		{
			auto comparator = [](std::reference_wrapper<const std::string> str1, std::reference_wrapper<const std::string> str2)
			{
				return impl::utils::CompareStringsIgnoreCase()(str1.get(), str2.get());
			};

			std::transform(ids.cbegin(), ids.cend(), std::back_inserter(_ids),
				[](const std::reference_wrapper<const std::string>& id)
			{
				return id.get();
			});

			std::set<std::reference_wrapper<const std::string>, decltype(comparator)> distinct(
				ids.cbegin(), ids.cend(), comparator);
			const std::string empty_str{};
			distinct.erase(empty_str);

			std::for_each(distinct.cbegin(), distinct.cend(), [this](const std::reference_wrapper<const std::string>& id)
			{
				by_id(id.get());
			});

			return *this;
		}

		template<typename T>
		std::shared_ptr<T> get_document(
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			if (_session->no_tracking)
			{
				if (!_current_load_results)
				{
					throw std::runtime_error("Cannot execute 'get_documents' before operation execution.");
				}
				if(_current_load_results)
				{
					
					auto document = _current_load_results.value().results.size() > 0 ?
						_current_load_results.value().results.at(0) : nlohmann::json(nullptr);
					if(document.is_null())
					{
						return  nullptr;
					}
					auto doc_info = DocumentInfo(document);
					return _session->track_entity<T>(doc_info, from_json, to_json);
				}else
				{
					return nullptr;
				}
			}
			return get_document<T>(_ids[0], from_json, to_json);//TODO check if _ids[0] is OK
		}
		template<typename T>
		std::shared_ptr<T> get_document(
			const std::string& id,
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			if(id.empty())
			{
				return {};
			}
			if(_session->is_deleted(id))
			{
				return {};
			}

			if (auto doc_info = _session->_documents_by_id.find(id);
				doc_info != _session->_documents_by_id.end())
			{
				return _session->track_entity<T>(*doc_info->second, from_json, to_json);
			}
			
			if (auto doc_info = _session->_included_documents_by_id.find(id);
				doc_info != _session->_included_documents_by_id.end())
			{
				return _session->track_entity<T>(*doc_info->second, from_json, to_json);
			}

			return {};
		}

		template<typename T>
		DocumentsByIdsMap<T> get_documents(
			const std::optional<DocumentInfo::FromJsonConverter>& from_json = {},
			const std::optional<DocumentInfo::ToJsonConverter>& to_json = {})
		{
			DocumentsByIdsMap<T> results{};

			if(_session->no_tracking)
			{
				if(!_current_load_results)
				{
					throw std::runtime_error("Cannot execute 'get_documents' before operation execution.");
				}
				std::for_each(_ids.cbegin(), _ids.cend(), [&](const std::string& id)
				{
					if(!id.empty())
					{
						results.insert_or_assign(id, nullptr);
					}
				});

				for(auto& doc: _current_load_results.value().results)
				{
					if(doc.is_null())
					{
						continue;
					}
					auto doc_info = DocumentInfo(doc);
					results.insert_or_assign(doc_info.id, _session->track_entity<T>(doc_info, from_json, to_json));
				}
			}

			for(const auto& id : _ids)
			{
				if(!id.empty())
				{
					results.insert_or_assign(id, get_document<T>(id, from_json, to_json));
				}
			}
			return results;
		}

		void set_result(const commands::GetDocumentsResult& result)
		{
			if(_session->no_tracking)
			{
				_current_load_results = result;
			}

			_session->register_includes(result.includes);

			//TODO take care of counters

			for(const auto& document : result.results)
			{
				if(document.is_null())
				{
					continue;
				}

				auto new_doc_info = std::make_shared<DocumentInfo>(document);
				_session->_documents_by_id.insert({new_doc_info->id,new_doc_info});
			}

			_session->register_missing_includes(result.results, result.includes, _includes);
		}
	};
}
