#include "stdafx.h"
#include "DocumentSessionImpl.h"
#include "BatchOperation.h"
#include "BatchCommand.h"
#include "LoadOperation.h"
#include "SessionOptions.h"
#include "LoaderWithInclude.h"
#include "PatchRequest.h"
#include "PatchCommandData.h"

namespace ravendb::client::documents::session
{
	DocumentSessionImpl::DocumentSessionImpl(DocumentStoreBase& document_store, SessionOptions options)
		: InMemoryDocumentSessionOperations(document_store, std::move(options))
	{}

	operations::LoadOperation DocumentSessionImpl::load_impl(const std::string& id)
	{
		auto load_op = operations::LoadOperation(*this);
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
		auto load_op = operations::LoadOperation(*this);
		load_internal(ids, load_op);

		return load_op;
	}

	operations::LoadOperation DocumentSessionImpl::load_impl(
		const std::vector<std::reference_wrapper<const std::string>>& ids,
		const std::vector<std::string>& includes)
	{
		auto load_op = operations::LoadOperation(*this);
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
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if(auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() && !doc_info_it->second->update_from_json)
		{
			doc_info_it->second->update_from_json = update_from_json;
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
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() && !doc_info_it->second->update_from_json)
		{
			doc_info_it->second->update_from_json = update_from_json;
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
		const DocumentInfo::EntityUpdater& update_from_json)
	{
		if (auto doc_info_it = _documents_by_id.find(id);
			doc_info_it != _documents_by_id.end() && !doc_info_it->second->update_from_json)
		{
			doc_info_it->second->update_from_json = update_from_json;
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

	loaders::LoaderWithInclude DocumentSessionImpl::include(const std::string& path)
	{
		return loaders::MultiLoaderWithInclude::create(*this).include(path);
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
		auto save_changes_operation = operations::BatchOperation(*this);
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

	std::shared_ptr<RawDocumentQuery> DocumentSessionImpl::raw_query(const std::string& query)
	{
		return RawDocumentQuery::create(*this, query);
	}

}
