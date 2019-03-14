#include "stdafx.h"
#include "DocumentSessionImpl.h"
#include "BatchOperation.h"
#include "BatchCommand.h"
#include "LoadOperation.h"
#include "SessionOptions.h"
#include "LoaderWithInclude.h"
#include "PatchRequest.h"
#include "PatchCommandData.h"
#include "SimpleStopWatch.h"
#include "MultiGetOperation.h"
#include "LazySessionOperations.h"

namespace ravendb::client::documents::session
{
	DocumentSessionImpl::DocumentSessionImpl(std::shared_ptr<DocumentStoreBase> document_store, SessionOptions options)
		: InMemoryDocumentSessionOperations(document_store, std::move(options))
	{}

	operations::lazy::LazySessionOperations DocumentSessionImpl::lazily()
	{
		return operations::lazy::LazySessionOperations(operations::lazy::LazySessionOperationsImpl::create(
			std::static_pointer_cast<DocumentSessionImpl>(_weak_this.lock())));
	}

	operations::LoadOperation DocumentSessionImpl::load_impl(const std::string& id)
	{
		auto load_op = operations::LoadOperation(_weak_this.lock());
		load_op.by_id(id);
		auto cmd = load_op.create_request();

		if (cmd)
		{
			_request_executor->execute(*cmd);
			load_op.set_result(cmd->get_result());
		}

		return load_op;
	}

	operations::LoadOperation DocumentSessionImpl::load_impl(const std::vector<std::reference_wrapper<const std::string>>& ids)
	{
		auto load_op = operations::LoadOperation(_weak_this.lock());
		load_internal(ids, load_op);

		return load_op;
	}

	operations::LoadOperation DocumentSessionImpl::load_impl(
		const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes)
	{
		auto load_op = operations::LoadOperation(_weak_this.lock());
		load_op.by_ids(ids);
		load_op.with_includes(includes);

		//TODO take care of counters;

		auto cmd = load_op.create_request();
		if (cmd)
		{
			_request_executor->execute(*cmd);
			load_op.set_result(cmd->get_result());
		}

		return load_op;
	}


	void DocumentSessionImpl::load_internal(const std::vector<std::reference_wrapper<const std::string>>& ids,
		operations::LoadOperation& operation)
	{
		operation.by_ids(ids);

		auto cmd = operation.create_request();

		if (cmd)
		{
			_request_executor->execute(*cmd);
			operation.set_result(cmd->get_result());
		}
	}

	void DocumentSessionImpl::patch_internal(const std::string& id, const std::string& path, const nlohmann::json& value,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{
		if(auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() &&
			update_from_json.has_value())
		{
			doc_info_it->second->update_from_json = *update_from_json;
		}

		auto patch_request = documents::operations::PatchRequest();
		std::ostringstream script{};
		script << "this." << path << " = args.val_" << _vals_count << ";";
		patch_request.script = script.str();
		patch_request.values.insert_or_assign("val_" + std::to_string(_vals_count), value);

		++_vals_count;

		if(!try_merge_patches(id, patch_request))
		{
			defer({ std::make_shared<commands::batches::PatchCommandData>(id, std::optional<std::string>(),
				patch_request, std::optional<documents::operations::PatchRequest>()) });
		}
	}

	void DocumentSessionImpl::patch_internal(const std::string& id,
		const std::string& script, const std::unordered_map<std::string, nlohmann::json>& values,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() &&
			update_from_json.has_value())
		{
			doc_info_it->second->update_from_json = *update_from_json;
		}

		auto patch_request = documents::operations::PatchRequest();
		patch_request.script = script;
		patch_request.values = values;

		if (!try_merge_patches(id, patch_request))
		{
			defer({ std::make_shared<commands::batches::PatchCommandData>(id, std::optional<std::string>(),
				patch_request, std::optional<documents::operations::PatchRequest>()) });
		}
	}


	void DocumentSessionImpl::increment_internal(const std::string& id, const std::string& path, const nlohmann::json& value_to_add,
		const std::optional<DocumentInfo::EntityUpdater>& update_from_json)
	{
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() &&
			update_from_json.has_value())
		{
			doc_info_it->second->update_from_json = *update_from_json;
		}

		auto patch_request = documents::operations::PatchRequest();
		const std::string variable = "this." + path;
		const std::string value = "args.val_" + std::to_string(_vals_count);

		std::ostringstream script{};
		script << variable << " = " << variable <<
			" ? " << variable << " + " << value <<
			" : " << value << ";";
		patch_request.script = script.str();
		patch_request.values.insert_or_assign("val_" + std::to_string(_vals_count), value_to_add);

		++_vals_count;

		if (!try_merge_patches(id, patch_request))
		{
			defer({ std::make_shared<commands::batches::PatchCommandData>(id, std::optional<std::string>(),
				patch_request, std::optional<documents::operations::PatchRequest>()) });
		}
	}

	bool DocumentSessionImpl::try_merge_patches(const std::string & id, const documents::operations::PatchRequest & patch_request)
	{
		std::shared_ptr<commands::batches::CommandDataBase> command{};

		if (auto command_it = _deferred_commands_map.find(in_memory_document_session_operations::IdTypeAndName
			{ id, commands::batches::CommandType::PATCH, {} });
			command_it == _deferred_commands_map.end())
		{
			return false;
		}else
		{
			command = command_it->second;
			auto remove_it = std::find(_deferred_commands.cbegin(), _deferred_commands.cend(), command);
			_deferred_commands.erase(remove_it);
			// We'll overwrite the _deferred_commands_map when calling defer
		}

		auto old_patch = std::static_pointer_cast<commands::batches::PatchCommandData>(command);
		std::ostringstream new_script{};
		new_script << old_patch->get_patch().script << "\n" << patch_request.script;

		decltype(documents::operations::PatchRequest::values) new_values(std::move(old_patch->get_patch().values));
		std::copy(patch_request.values.cbegin(), patch_request.values.cend(), std::inserter(new_values, new_values.end()));

		auto new_patch_request = documents::operations::PatchRequest(std::move(new_script).str());
		new_patch_request.values = std::move(new_values);

		defer({ std::make_shared<commands::batches::PatchCommandData>(id, std::optional<std::string>(),
			std::move(new_patch_request), std::optional<documents::operations::PatchRequest>()) });

		return true;
	}

	bool DocumentSessionImpl::execute_lazy_operations_single_step(ResponseTimeInformation& response_time_information,
		const std::vector<commands::multi_get::GetRequest>& requests)
	{
		auto multi_get_operation = operations::MultiGetOperation(_weak_this.lock());
		auto multi_get_command = multi_get_operation.create_request(requests);
		get_request_executor()->execute(multi_get_command);

		auto& responses = multi_get_command.get_result();

		for(auto i = 0; i <_pending_lazy_operations.size(); ++i)
		{
			int64_t total_time{};

			const commands::multi_get::GetResponse& response = responses[i];

			if(auto temp_req_time_it = response.headers.find(constants::headers::REQUEST_TIME);
				temp_req_time_it != response.headers.end())
			{
				total_time = std::strtoll(temp_req_time_it->second.c_str(), nullptr, 10);
			}

			auto time_item = ResponseTimeInformation::ResponseTimeItem();
			time_item.url = requests[i].get_url_and_query();
			time_item.duration = std::chrono::milliseconds(total_time);

			response_time_information.duration_break_down.push_back(std::move(time_item));

			if(response.request_has_errors())
			{
				std::ostringstream msg{};
				msg << "Got an error from server, status code: " <<
					response.status_code << "\r\n" << (response.result ? *response.result : "");
				throw std::runtime_error(msg.str());
			}
			_pending_lazy_operations[i]->handle_response(response);
			if(_pending_lazy_operations[i]->is_requires_retry())
			{
				return true;
			}
		}
		return false;
	}

	std::string DocumentSessionImpl::generate_id(std::type_index type, std::shared_ptr<void> entity) const
	{
		return get_conventions()->generate_document_id(database_name, type, entity);
	}

	DocumentSessionImpl::~DocumentSessionImpl() = default;

	std::shared_ptr<DocumentSessionImpl> DocumentSessionImpl::create(std::shared_ptr<DocumentStoreBase> document_store,
		SessionOptions options)
	{
		auto object = std::shared_ptr<DocumentSessionImpl>(new DocumentSessionImpl(document_store, options));
		object->_weak_this = object;
		object->initialize();
		return object;
	}

	loaders::LoaderWithInclude DocumentSessionImpl::include(const std::string& path)
	{
		return loaders::MultiLoaderWithInclude::create(std::static_pointer_cast<DocumentSessionImpl>(_weak_this.lock())).include(path);
	}

	ResponseTimeInformation DocumentSessionImpl::execute_all_pending_lazy_operations()
	{
		std::vector<commands::multi_get::GetRequest> requests{};
		for (auto i = 0; i < _pending_lazy_operations.size(); ++i)
		{
			auto&& req = _pending_lazy_operations[i]->create_request();
			if (!req)
			{
				_pending_lazy_operations.erase(_pending_lazy_operations.begin() + i);
				--i;
				continue;
			}
			requests.push_back(*std::move(req));
		}

		if (requests.empty())
		{
			return {};
		}

		impl::SimpleStopWatch sw{};

		increment_request_count();

		auto response_time_duration = ResponseTimeInformation{};

		while (execute_lazy_operations_single_step(response_time_duration, requests))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		response_time_duration.compute_server_total();

		for (const auto& pending_lazy_operation : _pending_lazy_operations)
		{
			if (auto value_it = _on_evaluate_lazy.find(pending_lazy_operation);
				value_it != _on_evaluate_lazy.end())
			{
				value_it->second();
			}
		}

		response_time_duration.total_client_duration = sw.millis_elapsed();
		_pending_lazy_operations.clear();
		return response_time_duration;
	}

	bool DocumentSessionImpl::exists(const std::string& id)
	{
		if (_known_missing_ids.find(id) != _known_missing_ids.end())
		{
			return false;
		}
		if (_documents_by_id.find(id) != _documents_by_id.end())
		{
			return true;
		}

		//TODO execute HeadDocumentCommand , for now->
		throw std::runtime_error("Not implemented");
	}

	void DocumentSessionImpl::save_changes()
	{
		auto save_changes_operation = operations::BatchOperation(_weak_this.lock());
		if (auto command = save_changes_operation.create_request();
			!command)
		{
			return;
		}
		else
		{
			if (no_tracking)
			{
				throw std::runtime_error("Cannot execute save_changes() when entity tracking is disabled in session.");
			}
			_request_executor->execute(*command);
			update_session_after_changes(command->get_result());
			save_changes_operation.set_result(command->get_result());
		}
	}
}
